"""
Core brain operations for NLM.

This module provides Pythonic access to the neural brain simulation system,
including context managers, property-based access, and modern Python patterns.
"""

import time
from typing import Any, Dict, List, Optional, Union, Iterator, ContextManager
from contextlib import contextmanager

from ..bindings import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    Config, Vision, Audio, InternalSignals, SensoryInput,
    Action, WorldObject, AgentBody, ActionResult,
    SensoryPercept, SimpleWorld, Brain, AgentBrain,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
)

from .exceptions import (
    NLMError, ConfigurationError, SimulationError, LearningError, VisualizationError
)

__all__ = [
    # Core classes
    "Brain", "Config", "SimpleWorld", "AgentBrain",
    # Sensory classes
    "Vision", "Audio", "InternalSignals", "SensoryInput", "SensoryPercept",
    # Motor classes
    "Action", "WorldObject", "AgentBody", "ActionResult",
    # Type definitions
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    # Factory functions
    "create_default_config", "create_brain", "create_simple_world", "create_agent_brain",
    # Context manager
    "brain_context",
    # Exceptions
    "NLMError", "ConfigurationError", "SimulationError", "LearningError", "VisualizationError",
]

# Pythonic context managers
@contextmanager
def brain_context(config: Optional[Config] = None) -> Iterator[Brain]:
    """
    Context manager for brain simulation.
    
    Automatically initializes and resets the brain.
    
    Args:
        config: Configuration for the brain. If None, uses default config.
    
    Yields:
        A Brain instance that is properly initialized and will be reset on exit.
    """
    if config is None:
        config = create_default_config()
    
    brain = create_brain(config)
    if not brain.initialize():
        raise SimulationError("Failed to initialize brain")
    
    try:
        yield brain
    finally:
        brain.reset()

# Enhanced brain class with Pythonic API
class EnhancedBrain:
    """
    Enhanced Python API for the Brain class with modern Python patterns.
    
    Provides property-based access, context managers, and Pythonic iterators
    for better developer experience while maintaining performance.
    """
    
    def __init__(self, brain: Brain):
        self._brain = brain
        self._initialized = False
    
    def __enter__(self) -> 'EnhancedBrain':
        """Context manager entry."""
        self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Context manager exit."""
        self.reset()
    
    def initialize(self) -> bool:
        """Initialize the brain."""
        self._initialized = self._brain.initialize()
        return self._initialized
    
    def reset(self) -> None:
        """Reset the brain state."""
        self._brain.reset()
        self._initialized = False
    
    @property
    def initialized(self) -> bool:
        """Check if the brain is initialized."""
        return self._initialized
    
    # Property-based access to brain statistics
    @property
    def region_count(self) -> int:
        """Get the number of regions in the brain."""
        return self._brain.getRegionCount()
    
    @property
    def neuron_count(self) -> int:
        """Get the total number of neurons in the brain."""
        return self._brain.getTotalNeuronCount()
    
    @property
    def synapse_count(self) -> int:
        """Get the total number of synapses in the brain."""
        return self._brain.getTotalSynapseCount()
    
    @property
    def active_neuron_count(self) -> int:
        """Get the number of active neurons."""
        return self._brain.getActiveNeuronCount()
    
    @property
    def firing_neuron_count(self) -> int:
        """Get the number of firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    @property
    def average_firing_rate(self) -> float:
        """Get the average firing rate."""
        return self._brain.getAverageFiringRate()
    
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get the excitation/inhibition ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    @property
    def total_spike_count(self) -> int:
        """Get the total spike count."""
        return self._brain.getTotalSpikeCount()
    
    @property
    def developmental_stage(self) -> DevelopmentalStage:
        """Get the current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    @developmental_stage.setter
    def developmental_stage(self, stage: DevelopmentalStage) -> None:
        """Set the developmental stage."""
        self._brain.setDevelopmentalStage(stage)
    
    # Pythonic iteration over regions
    def iterate_regions(self) -> Iterator[RegionId]:
        """
        Iterate over all region IDs.
        
        Yields:
            RegionId objects for each region in the brain.
        """
        region_ids = self._brain.getRegionIds()
        for region_id in region_ids:
            yield region_id
    
    # Pythonic factory methods
    def add_region(self, name: str = "") -> RegionId:
        """
        Add a new neural region.
        
        Args:
            name: Optional name for the region.
        
        Returns:
            The RegionId of the newly created region.
        """
        return self._brain.addRegion(name)
    
    def get_region(self, region_id: RegionId) -> Any:
        """
        Get a region by ID.
        
        Args:
            region_id: The ID of the region to retrieve.
        
        Returns:
            The region object.
        """
        return self._brain.getRegion(region_id)
    
    # Pythonic simulation methods
    def step(self, timestep: int) -> None:
        """
        Perform a simulation step.
        
        Args:
            timestep: The current simulation timestep.
        """
        self._brain.step(timestep)
    
    def step_with_time(self, timestep: int, current_time: float) -> None:
        """
        Perform a simulation step with timestamp.
        
        Args:
            timestep: The current simulation timestep.
            current_time: The current simulation time.
        """
        self._brain.step(timestep, current_time)
    
    # Pythonic data access
    def inject_sensory_input(self, input_data: SensoryInput) -> None:
        """
        Inject sensory input into the brain.
        
        Args:
            input_data: The sensory input data.
        """
        self._brain.receiveSensoryInput(input_data)
    
    def inject_current(self, neuron_id: NeuronId, current: float) -> None:
        """
        Inject current into a specific neuron.
        
        Args:
            neuron_id: The ID of the neuron to inject current into.
            current: The current value to inject.
        """
        self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neuron_type(self, neuron_type: NeuronType, current: float) -> None:
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: The type of neurons to inject current into.
            current: The current value to inject.
        """
        self._brain.injectCurrentToNeurons(neuron_type, current)
    
    # Save/load functionality
    def save(self, filepath: str) -> None:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save the brain state.
        """
        self._brain.save(filepath)
    
    def load(self, filepath: str) -> None:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load the brain state from.
        """
        self._brain.load(filepath)
    
    # Pythonic string representations
    def __repr__(self) -> str:
        """Get a string representation of the brain."""
        return f"<EnhancedBrain neuron_count={self.neuron_count}, region_count={self.region_count}>"
    
    def __str__(self) -> str:
        """Get a human-readable string representation."""
        return (f"EnhancedBrain(neuron_count={self.neuron_count}, "
                f"synapse_count={self.synapse_count}, "
                f"active_neurons={self.active_neuron_count}, "
                f"stage={self.developmental_stage})")
