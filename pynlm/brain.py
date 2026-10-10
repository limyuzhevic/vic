"""
Pythonic interface for NLM brain simulation.

This module provides a high-level interface to the NLM neural simulation
brain system, wrapping the low-level C++ Brain class with Pythonic methods,
type hints, and convenience functions for common operations.
"""

from typing import Optional, Dict, Any, List, Union
from dataclasses import dataclass
import numpy as np

# Import the C++ extension module directly
import pynlm as _c_extension
from .core import Config, ConfigError
class BrainError(Exception):
    """Raised when brain operations fail."""
    pass
@dataclass
class BrainStats:
    """Statistics about brain state.
    
    Attributes:
        total_neurons: Total number of neurons in the brain
        total_synapses: Total number of synapses
        active_neurons: Number of currently active neurons
        firing_neurons: Number of currently firing neurons
        average_firing_rate: Average firing rate across all neurons
        excitation_inhibition_ratio: Balance between excitation and inhibition
        total_spikes: Total number of spikes recorded
        current_developmental_stage: Current developmental stage
    """
    total_neurons: int
    total_synapses: int
    active_neurons: int
    firing_neurons: int
    average_firing_rate: float
    excitation_inhibition_ratio: float
    total_spikes: int
    current_developmental_stage: str
class Brain:
    """Central neural simulation brain class.
    
    The Brain class represents the core neural processing unit of the NLM system.
    It contains neurons, synapses, regions, and manages the neural dynamics,
    plasticity, development, and cognitive processes.
    
    The brain operates in discrete simulation steps and provides interfaces
    for receiving sensory input, producing motor output, and maintaining
    internal state through memory systems.
    
    Examples:
        >>> from pynlm import Brain, Config
        >>> config = Config(neuron_count=100, region_count=2)
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     brain.step(step)
        >>> 
        >>> # Get statistics
        >>> stats = brain.get_statistics()
        >>> print(f"Active neurons: {stats.active_neurons}")
    """
    
    def __init__(self, config: Optional[Config] = None) -> None:
        """Create a brain with optional configuration.
        
        Args:
            config: Configuration for the brain. If None, uses default config.
            
        Examples:
            >>> brain = Brain()  # Default configuration
            >>> brain = Brain(Config(neuron_count=50))  # Custom config
        """
        self._config = config or Config()
        self._brain = _c_extension.Brain(self._config._config)
        self._is_initialized = False
    
    def initialize(self) -> 'Brain':
        """Initialize the brain with current configuration.
        
        This sets up the neural architecture, creates regions, neurons, and
        synapses according to the configuration, and prepares the brain for
        simulation.
        
        Returns:
            Self for method chaining.
            
        Raises:
            BrainError: If initialization fails.
            
        Examples:
            >>> brain.initialize()
            >>> print("Brain initialized successfully")
        """
        try:
            self._brain.initialize()
            self._is_initialized = True
            return self
        except Exception as e:
            raise BrainError(f"Failed to initialize brain: {e}")
    
    def step(self, current_step: int, current_time: Optional[float] = None) -> None:
        """Perform a simulation step.
        
        Args:
            current_step: Current simulation step number.
            current_time: Optional current simulation time in milliseconds.
            
        Raises:
            BrainError: If brain is not initialized or step fails.
            
        Examples:
            >>> for step in range(100):
            ...     brain.step(step, step * 10.0)
        """
        if not self._is_initialized:
            raise BrainError("Brain must be initialized before calling step()")
        
        try:
            if current_time is not None:
                self._brain.step(current_step, current_time)
            else:
                self._brain.step(current_step)
        except Exception as e:
            raise BrainError(f"Failed to perform brain step: {e}")
    
    def receive_sensory_input(self, input_data: Any) -> None:
        """Inject sensory input into the brain.
        
        Args:
            input_data: Sensory input data. Can be a Vision, Audio,
                InternalSignals object, or compatible data structure.
                
        Raises:
            BrainError: If input injection fails.
            
        Examples:
            >>> from pynlm import Vision
            >>> vision = Vision(64, 64)
            >>> brain.receive_sensory_input(vision)
        """
        try:
            self._brain.receiveSensoryInput(input_data)
        except Exception as e:
            raise BrainError(f"Failed to receive sensory input: {e}")
    
    def inject_current(self, neuron_id: Any, current: float) -> None:
        """Inject current into a specific neuron.
        
        Args:
            neuron_id: ID of the neuron to inject current into.
            current: Current value to inject (in nanoamps).
            
        Raises:
            BrainError: If current injection fails.
            
        Examples:
            >>> brain.inject_current(neuron_id, 5.0)
        """
        try:
            self._brain.injectCurrent(neuron_id, current)
        except Exception as e:
            raise BrainError(f"Failed to inject current: {e}")
    
    def inject_current_to_neurons(self, neuron_type: Any, current: float) -> None:
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to inject current into.
            current: Current value to inject (in nanoamps).
            
        Raises:
            BrainError: If current injection fails.
            
        Examples:
            >>> from pynlm import NeuronType
            >>> brain.inject_current_to_neurons(NeuronType.Excitatory, 2.0)
        """
        try:
            self._brain.injectCurrentToNeurons(neuron_type, current)
        except Exception as e:
            raise BrainError(f"Failed to inject current to neurons: {e}")
    
    def produce_action(self) -> Any:
        """Produce motor action based on neural activity.
        
        Returns:
            Action object representing the motor output.
            
        Raises:
            BrainError: If action production fails.
            
        Examples:
            >>> action = brain.produce_action()
            >>> print(f"Action type: {action.getType()}")
        """
        try:
            return self._brain.produceAction()
        except Exception as e:
            raise BrainError(f"Failed to produce action: {e}")
    
    def reset(self) -> 'Brain':
        """Reset brain state to initial conditions.
        
        This clears all neural activity, resets neuron states, and
        prepares the brain for a new simulation episode.
        
        Returns:
            Self for method chaining.
            
        Raises:
            BrainError: If reset fails.
            
        Examples:
            >>> brain.reset()
            >>> brain.initialize()
        """
        try:
            self._brain.reset()
            self._is_initialized = False
            return self
        except Exception as e:
            raise BrainError(f"Failed to reset brain: {e}")
    
    def save(self, filepath: Union[str, Path]) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: Path where to save the brain state.
            
        Returns:
            True if successful, False otherwise.
            
        Raises:
            BrainError: If save fails.
            
        Examples:
            >>> brain.save("brain_state.nml")
        """
        from pathlib import Path
        try:
            path = Path(filepath)
            path.parent.mkdir(parents=True, exist_ok=True)
            return self._brain.save(str(path))
        except Exception as e:
            raise BrainError(f"Failed to save brain state: {e}")
    
    def load(self, filepath: Union[str, Path]) -> 'Brain':
        """Load brain state from file.
        
        Args:
            filepath: Path to the brain state file.
            
        Returns:
            Self for method chaining.
            
        Raises:
            BrainError: If load fails.
            
        Examples:
            >>> brain.load("brain_state.nml")
        """
        from pathlib import Path
        try:
            path = Path(filepath)
            if not path.exists():
                raise BrainError(f"Brain state file not found: {filepath}")
            
            self._brain.load(str(path))
            self._is_initialized = True
            return self
        except Exception as e:
            raise BrainError(f"Failed to load brain state: {e}")
    
    # Region management
    def add_region(self, name: str = "") -> Any:
        """Add a new neural region.
        
        Args:
            name: Optional name for the region.
            
        Returns:
            Region ID of the newly created region.
            
        Raises:
            BrainError: If region creation fails.
            
        Examples:
            >>> region_id = brain.add_region("sensory_cortex")
        """
        try:
            return self._brain.addRegion(name)
        except Exception as e:
            raise BrainError(f"Failed to add region: {e}")
    
    def get_region(self, region_id: Any) -> Any:
        """Get a region by ID.
        
        Args:
            region_id: ID of the region to retrieve.
            
        Returns:
            Region object or None if not found.
            
        Examples:
            >>> region = brain.get_region(region_id)
        """
        try:
            return self._brain.getRegion(region_id)
        except Exception:
            return None
    
    def get_region_count(self) -> int:
        """Get the number of regions.
        
        Returns:
            Number of regions in the brain.
            
        Examples:
            >>> count = brain.get_region_count()
        """
        return self._brain.getRegionCount()
    
    def get_region_ids(self) -> List[Any]:
        """Get all region IDs.
        
        Returns:
            List of all region IDs.
            
        Examples:
            >>> region_ids = brain.get_region_ids()
        """
        return self._brain.getRegionIds()
    
    def get_regions(self) -> List[Any]:
        """Get all regions.
        
        Returns:
            List of all region objects.
            
        Examples:
            >>> regions = brain.get_regions()
        """
        return self._brain.getRegions()
    
    # Statistics and monitoring
    def get_statistics(self) -> BrainStats:
        """Get comprehensive brain statistics.
        
        Returns:
            BrainStats object containing current statistics.
            
        Examples:
            >>> stats = brain.get_statistics()
            >>> print(f"Total neurons: {stats.total_neurons}")
        """
        return BrainStats(
            total_neurons=self._brain.getTotalNeuronCount(),
            total_synapses=self._brain.getTotalSynapseCount(),
            active_neurons=self._brain.getActiveNeuronCount(),
            firing_neurons=self._brain.getFiringNeuronCount(),
            average_firing_rate=self._brain.getAverageFiringRate(),
            excitation_inhibition_ratio=self._brain.getExcitationInhibitionRatio(),
            total_spikes=self._brain.getTotalSpikeCount(),
            current_developmental_stage=self.get_developmental_stage()
        )
    
    def log_status(self) -> None:
        """Log current brain status and statistics.
        
        Examples:
            >>> brain.log_status()
        """
        self._brain.logStatus()
    
    # Development and neuromodulation
    def get_developmental_stage(self) -> Any:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage enum value.
            
        Examples:
            >>> stage = brain.get_developmental_stage()
        """
        return self._brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage: Any) -> None:
        """Set developmental stage.
        
        Args:
            stage: Developmental stage enum value.
            
        Examples:
            >>> from pynlm import DevelopmentalStage
            >>> brain.set_developmental_stage(DevelopmentalStage.Maturation)
        """
        self._brain.setDevelopmentalStage(stage)
    
    def get_config(self) -> Config:
        """Get the brain's configuration.
        
        Returns:
            Config object used by this brain.
            
        Examples:
            >>> config = brain.get_config()
        """
        return self._config
    
    def is_initialized(self) -> bool:
        """Check if brain is initialized.
        
        Returns:
            True if brain has been initialized, False otherwise.
            
        Examples:
            >>> if brain.is_initialized():
            ...     print("Brain is ready for simulation")
        """
        return self._is_initialized
    
    def __repr__(self) -> str:
        """String representation of brain."""
        status = "initialized" if self._is_initialized else "not initialized"
        return f"<Brain: {status}, {self.get_region_count()} regions>"
    
    def __str__(self) -> str:
        """String representation of brain."""
        stats = self.get_statistics()
        return f"Brain(neurons={stats.total_neurons}, regions={self.get_region_count()})"
# Convenience function
def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with configuration.
    
    Args:
        config: Configuration for the brain. If None, uses default config.
        
    Returns:
        New Brain instance, initialized with the configuration.
        
    Examples:
        >>> brain = create_brain()
        >>> brain = create_brain(Config(neuron_count=50))
    """
    brain_instance = Brain(config)
    brain_instance.initialize()
    return brain_instance