"""
NLM Brain implementation.

This module provides the central neural simulation brain class that coordinates
all neural processing, memory systems, prediction, cognition, neuromodulation,
and development. The Brain class is the main component of NLM - it contains
neurons, synapses, populations, and regions, along with all the integrated
systems that make up the artificial brain.
"""

import json
import time
from typing import Dict, List, Optional, Any, Union

from .config import Config, EnhancedConfig

# Import C++ bindings
from .bindings import (
    Brain as _Brain,
    Config as _Config,
    Config as ConfigClass,
)
class EnhancedBrain:
    """Enhanced Brain class with Pythonic properties and better error handling.
    
    This class provides a Pythonic interface to the NLM brain simulation,
    making it easier to work with neural networks through properties and
    methods that follow Python conventions.
    
    The enhanced brain provides:
    - Pythonic property access to brain state
    - Better error handling with custom exceptions
    - Convenience methods for common operations
    - Simulation step statistics and logging
    - Better debugging and monitoring capabilities
    
    Example:
        >>> config = EnhancedConfig()
        >>> brain = EnhancedBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation with statistics
        >>> for step in range(100):
        ...     brain.step(step)
        ...     print(f"Step {step}: {brain.firing_neuron_count} neurons firing")
        >>> 
        >>> # Access brain statistics
        >>> print(f"Total neurons: {brain.total_neuron_count}")
        >>> print(f"Total spikes: {brain.total_spike_count}")
    """
    
    def __init__(self, config: Union[EnhancedConfig, Config, Any]):
        """Initialize brain with enhanced config.
        
        Args:
            config: EnhancedConfig or Config object containing brain configuration.
            
        Raises:
            TypeError: If config is not a valid configuration object.
            ConfigurationError: If configuration is invalid.
        """
        try:
            if isinstance(config, EnhancedConfig):
                self._config = config
            elif isinstance(config, Config):
                self._config = config
            else:
                # Try to convert to EnhancedConfig
                self._config = EnhancedConfig(config)
            
            self._brain = _Brain(self._config._config if isinstance(config, EnhancedConfig) else config)
            self._initialized = False
            
        except Exception as e:
            raise SimulationError(f"Failed to create brain: {e}")
    
    def initialize(self) -> bool:
        """Initialize the brain.
        
        Returns:
            True if successful, False otherwise.
            
        Raises:
            SimulationError: If brain initialization fails.
        """
        try:
            self._initialized = self._brain.initialize()
            return self._initialized
        except Exception as e:
            raise SimulationError(f"Failed to initialize brain: {e}")
    
    def step(self, step_number: int, time: Optional[float] = None) -> None:
        """Perform a simulation step.
        
        Args:
            step_number: Step number for simulation.
            time: Optional timestamp for the step.
            
        Raises:
            SimulationError: If brain step fails or brain is not initialized.
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before stepping")
        
        try:
            if time is not None:
                self._brain.step(step_number, time)
            else:
                self._brain.step(step_number)
                
        except Exception as e:
            raise SimulationError(f"Failed to step brain: {e}")
    
    def step_with_time(self, step_number: int, time: float) -> None:
        """Perform a simulation step with timestamp.
        
        Args:
            step_number: Step number for simulation.
            time: Timestamp for the step.
        """
        self.step(step_number, time)
    
    def inject_current(self, neuron_id: Any, current: float) -> None:
        """Inject current into a specific neuron.
        
        Args:
            neuron_id: Neuron ID to inject current into.
            current: Current value to inject (mA).
            
        Raises:
            SimulationError: If injection fails.
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before injecting current")
        
        try:
            self._brain.injectCurrent(neuron_id, current)
        except Exception as e:
            raise SimulationError(f"Failed to inject current into neuron {neuron_id}: {e}")
    
    def inject_current_to_neurons(self, neuron_type: Any, current: float) -> None:
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to inject current into.
            current: Current value to inject (mA).
            
        Raises:
            SimulationError: If injection fails.
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before injecting current")
        
        try:
            self._brain.injectCurrentToNeurons(neuron_type, current)
        except Exception as e:
            raise SimulationError(f"Failed to inject current to neurons of type {neuron_type}: {e}")
    
    def receiveSensoryInput(self, input_data: Any) -> None:
        """Inject sensory input into the brain.
        
        Args:
            input_data: Sensory input data to inject.
            
        Raises:
            SimulationError: If input injection fails.
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before receiving input")
        
        try:
            self._brain.receiveSensoryInput(input_data)
        except Exception as e:
            raise SimulationError(f"Failed to receive sensory input: {e}")
    
    def produceAction(self) -> Any:
        """Produce motor action based on neural activity.
        
        Returns:
            Motor action based on brain activity.
            
        Raises:
            SimulationError: If action production fails.
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before producing action")
        
        try:
            return self._brain.produceAction()
        except Exception as e:
            raise SimulationError(f"Failed to produce action: {e}")
    
    def reset(self) -> None:
        """Reset brain state.
        
        Raises:
            SimulationError: If reset fails.
        """
        try:
            self._brain.reset()
            self._initialized = False
        except Exception as e:
            raise SimulationError(f"Failed to reset brain: {e}")
    
    def save(self, filepath: str) -> None:
        """Save brain state to file.
        
        Args:
            filepath: Path to save brain state.
            
        Raises:
            SerializationError: If file I/O fails.
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before saving")
        
        try:
            self._brain.save(filepath)
        except Exception as e:
            raise SerializationError(f"Failed to save brain to '{filepath}': {e}")
    
    def load(self, filepath: str) -> None:
        """Load brain state from file.
        
        Args:
            filepath: Path to load brain state from.
            
        Raises:
            SerializationError: If file I/O fails.
        """
        try:
            self._brain.load(filepath)
            self._initialized = True
        except Exception as e:
            raise SerializationError(f"Failed to load brain from '{filepath}': {e}")
    
    def addRegion(self, name: str = "") -> Any:
        """Add a new neural region.
        
        Args:
            name: Name of the region to add.
            
        Returns:
            Region ID of the newly added region.
            
        Raises:
            SimulationError: If region addition fails.
        """
        try:
            return self._brain.addRegion(name)
        except Exception as e:
            raise SimulationError(f"Failed to add region '{name}': {e}")
    
    def getRegion(self, region_id: Any) -> Any:
        """Get a region by ID.
        
        Args:
            region_id: ID of the region to retrieve.
            
        Returns:
            Region object.
            
        Raises:
            SimulationError: If region retrieval fails.
        """
        try:
            return self._brain.getRegion(region_id)
        except Exception as e:
            raise SimulationError(f"Failed to get region {region_id}: {e}")
    
    def getRegionCount(self) -> int:
        """Get the number of regions.
        
        Returns:
            Number of regions in the brain.
        """
        try:
            return self._brain.getRegionCount()
        except Exception as e:
            raise SimulationError(f"Failed to get region count: {e}")
    
    def getRegionIds(self) -> List[Any]:
        """Get all region IDs.
        
        Returns:
            List of region IDs.
        """
        try:
            return self._brain.getRegionIds()
        except Exception as e:
            raise SimulationError(f"Failed to get region IDs: {e}")
    
    def getRegions(self) -> List[Any]:
        """Get all regions.
        
        Returns:
            List of all regions.
        """
        try:
            return self._brain.getRegions()
        except Exception as e:
            raise SimulationError(f"Failed to get regions: {e}")
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions.
        
        Returns:
            Total neuron count.
        """
        try:
            return self._brain.getTotalNeuronCount()
        except Exception as e:
            raise SimulationError(f"Failed to get total neuron count: {e}")
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions.
        
        Returns:
            Total synapse count.
        """
        try:
            return self._brain.getTotalSynapseCount()
        except Exception as e:
            raise SimulationError(f"Failed to get total synapse count: {e}")
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons.
        
        Returns:
            Number of active neurons.
        """
        try:
            return self._brain.getActiveNeuronCount()
        except Exception as e:
            raise SimulationError(f"Failed to get active neuron count: {e}")
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons.
        
        Returns:
            Number of firing neurons.
        """
        try:
            return self._brain.getFiringNeuronCount()
        except Exception as e:
            raise SimulationError(f"Failed to get firing neuron count: {e}")
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons.
        
        Returns:
            Average firing rate (Hz).
        """
        try:
            return self._brain.getAverageFiringRate()
        except Exception as e:
            raise SimulationError(f"Failed to get average firing rate: {e}")
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio.
        
        Returns:
            Excitation/inhibition ratio.
        """
        try:
            return self._brain.getExcitationInhibitionRatio()
        except Exception as e:
            raise SimulationError(f"Failed to get E/I ratio: {e}")
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count.
        
        Returns:
            Total number of spikes.
        """
        try:
            return self._brain.getTotalSpikeCount()
        except Exception as e:
            raise SimulationError(f"Failed to get total spike count: {e}")
    
    def getDevelopmentalStage(self) -> Any:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage.
        """
        try:
            return self._brain.getDevelopmentalStage()
        except Exception as e:
            raise SimulationError(f"Failed to get developmental stage: {e}")
    
    def setDevelopmentalStage(self, stage: Any) -> None:
        """Set developmental stage.
        
        Args:
            stage: Developmental stage to set.
            
        Raises:
            SimulationError: If stage setting fails.
        """
        try:
            self._brain.setDevelopmentalStage(stage)
        except Exception as e:
            raise SimulationError(f"Failed to set developmental stage: {e}")
    
    def getConfig(self) -> Union[EnhancedConfig, Config]:
        """Get the configuration.
        
        Returns:
            Configuration object used to create this brain.
        """
        try:
            config_obj = self._brain.getConfig()
            # Return the same type as was used to create the brain
            if isinstance(self._config, EnhancedConfig):
                return self._config
            else:
                return self._config
        except Exception as e:
            raise SimulationError(f"Failed to get brain configuration: {e}")
    
    def logStatus(self) -> str:
        """Log brain status.
        
        Returns:
            Status information as string.
        """
        try:
            return self._brain.logStatus()
        except Exception as e:
            raise SimulationError(f"Failed to log brain status: {e}")
    
    # Pythonic properties for easier access
    
    @property
    def total_neuron_count(self) -> int:
        """Total number of neurons in the brain."""
        return self.getTotalNeuronCount()
    
    @property
    def total_synapse_count(self) -> int:
        """Total number of synapses in the brain."""
        return self.getTotalSynapseCount()
    
    @property
    def active_neuron_count(self) -> int:
        """Number of currently active neurons."""
        return self.getActiveNeuronCount()
    
    @property
    def firing_neuron_count(self) -> int:
        """Number of currently firing neurons."""
        return self.getFiringNeuronCount()
    
    @property
    def average_firing_rate(self) -> float:
        """Average firing rate across all neurons (Hz)."""
        return self.getAverageFiringRate()
    
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Excitation/inhibition balance ratio."""
        return self.getExcitationInhibitionRatio()
    
    @property
    def total_spike_count(self) -> int:
        """Total number of spikes generated."""
        return self.getTotalSpikeCount()
    
    @property
    def developmental_stage(self) -> Any:
        """Current developmental stage."""
        return self.getDevelopmentalStage()
    
    @property
    def region_count(self) -> int:
        """Number of regions in the brain."""
        return self.getRegionCount()
    
    @property
    def initialized(self) -> bool:
        """Whether the brain has been initialized."""
        return self._initialized
    
    @property
    def config(self) -> Union[EnhancedConfig, Config]:
        """Get the configuration object used to create this brain."""
        return self.getConfig()
    
    def __repr__(self) -> str:
        return f"EnhancedBrain(neurons={self.total_neuron_count}, regions={self.region_count}, initialized={self.initialized})"
    
    def __str__(self) -> str:
        return self.logStatus()
class Brain(EnhancedBrain):
    """Brain class for NLM simulation.
    
    This is the main neural simulation brain class that coordinates all
    neural processing, memory systems, prediction, cognition, neuromodulation,
    and development. The Brain class contains neurons, synapses, populations,
    and regions, along with all the integrated systems that make up the
    artificial brain.
    
    The class provides access to the complete NLM neural simulation framework,
    including working memory, episodic memory, prediction systems, cognition,
    neuromodulation, and development systems.
    
    Example:
        >>> # Create brain with default configuration
        >>> config = Config()
        >>> brain = Brain(config)
        >>> 
        >>> # Initialize and run simulation
        >>> brain.initialize()
        >>> for step in range(100):
        ...     brain.step(step)
        >>> 
        >>> # Access brain statistics
        >>> print(f"Neurons: {brain.getTotalNeuronCount()}")
        >>> print(f"Spikes: {brain.getTotalSpikeCount()}")
    """
    
    def __init__(self, config: Union[EnhancedConfig, Config]):
        """Initialize brain with configuration.
        
        Args:
            config: EnhancedConfig or Config object containing brain configuration.
        """
        super().__init__(config)
    
    # The enhanced brain provides all the functionality through inheritance
    # This class is kept for backward compatibility and clarity


# Type aliases for better documentation
SimulationStep = int
Timestamp = float
NeuronID = Any
ActionType = Any

__all__ = ["Brain", "EnhancedBrain", "SimulationStep", "Timestamp", "NeuronID", "ActionType"]
