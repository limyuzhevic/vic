"""
Enhanced brain class for NLM with Pythonic interface.

This module provides an enhanced Python wrapper for the NLM brain class,
adding context managers, better error handling, and convenient methods.
"""

import time
from typing import Dict, Any, Optional, List, Union, ContextManager
from contextlib import contextmanager
from .exceptions import BrainError, NLMError

# Import the original C++ classes
import pynlm

def create_brain(config: Optional[pynlm.Config] = None) -> pynlm.Brain:
    """
    Create a new brain with configuration.
    
    Args:
        config: Configuration for the brain. If None, uses default config.
        
    Returns:
        Brain instance
    """
    if config is None:
        config = pynlm.createDefaultConfig()
    return pynlm.createBrain(config)

class BrainWrapper:
    """
    Enhanced Python wrapper for NLM brain with context manager support and better API.
    
    This class provides a more Pythonic interface to the brain system,
    including automatic initialization, error handling, and convenient methods.
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize the brain wrapper.
        
        Args:
            brain: The underlying C++ Brain object
        """
        self._brain = brain
    
    def initialize(self) -> bool:
        """
        Initialize the brain with configuration.
        
        Returns:
            True if initialization succeeded, False otherwise
            
        Raises:
            BrainError: If initialization fails
        """
        try:
            self._brain.initialize()
            return True
        except Exception as e:
            raise BrainError(f"Brain initialization failed: {e}")
    
    def step(self, step_number: int, time: Optional[float] = None) -> None:
        """
        Perform a simulation step.
        
        Args:
            step_number: The current step number
            time: Optional timestamp (if None, uses step_number)
            
        Raises:
            BrainError: If step fails
        """
        try:
            if time is not None:
                self._brain.step(step_number, time)
            else:
                self._brain.step(step_number)
        except Exception as e:
            raise BrainError(f"Brain step failed: {e}")
    
    def process_sensory_input(self, sensory_input) -> None:
        """
        Process sensory input and inject into the brain.
        
        Args:
            sensory_input: Sensory input data
            
        Raises:
            BrainError: If processing fails
        """
        try:
            self._brain.receiveSensoryInput(sensory_input)
        except Exception as e:
            raise BrainError(f"Sensory input processing failed: {e}")
    
    def produce_action(self) -> Any:
        """
        Produce a motor action based on neural activity.
        
        Returns:
            Action produced by the brain
            
        Raises:
            BrainError: If action production fails or no action is produced
        """
        try:
            action = self._brain.produceAction()
            if action is None:
                raise BrainError("Brain produced no action")
            return action
        except Exception as e:
            raise BrainError(f"Action production failed: {e}")
    
    def reset(self) -> None:
        """Reset brain state."""
        self._brain.reset()
    
    def save(self, filepath: str) -> None:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file
        """
        self._brain.save(filepath)
    
    def load(self, filepath: str) -> None:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file
        """
        self._brain.load(filepath)
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get brain statistics as a dictionary.
        
        Returns:
            Dictionary of brain statistics
        """
        return {
            'neuron_count': self._brain.getTotalNeuronCount(),
            'synapse_count': self._brain.getTotalSynapseCount(),
            'active_neuron_count': self._brain.getActiveNeuronCount(),
            'firing_neuron_count': self._brain.getFiringNeuronCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'total_spike_count': self._brain.getTotalSpikeCount(),
            'developmental_stage': str(self._brain.getDevelopmentalStage()),
        }
    
    def inject_current(self, neuron_id: Any, current: float) -> None:
        """
        Inject current into a specific neuron.
        
        Args:
            neuron_id: ID of the neuron
            current: Current value to inject
        """
        self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type: Any, current: float) -> None:
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to target
            current: Current value to inject
        """
        self._brain.injectCurrentToNeurons(neuron_type, current)
    
    def add_region(self, name: str = "") -> Any:
        """
        Add a new neural region.
        
        Args:
            name: Optional region name
            
        Returns:
            Region ID
        """
        return self._brain.addRegion(name)
    
    def get_region(self, region_id: Any) -> Any:
        """
        Get a region by ID.
        
        Args:
            region_id: Region ID
            
        Returns:
            Region object
        """
        return self._brain.getRegion(region_id)
    
    def get_region_count(self) -> int:
        """Get the number of regions."""
        return self._brain.getRegionCount()
    
    def get_region_ids(self) -> List[Any]:
        """Get all region IDs."""
        return self._brain.getRegionIds()
    
    def get_regions(self) -> List[Any]:
        """Get all regions."""
        return self._brain.getRegions()
    
    def set_developmental_stage(self, stage: Any) -> None:
        """
        Set the developmental stage.
        
        Args:
            stage: Developmental stage
        """
        self._brain.setDevelopmentalStage(stage)
    
    def get_developmental_stage(self) -> Any:
        """Get the current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def log_status(self) -> None:
        """Log brain status."""
        self._brain.logStatus()
    
    @contextmanager
    def step_context(self, step_number: int, time: Optional[float] = None) -> ContextManager:
        """
        Context manager for a simulation step.
        
        Args:
            step_number: The current step number
            time: Optional timestamp
            
        Yields:
            self
            
        Example:
            with brain.step_context(step_number=100) as brain:
                brain.process_sensory_input(sensory_data)
                action = brain.produce_action()
        """
        try:
            yield self
        finally:
            pass  # Cleanup could go here

# Context manager for Brain
@contextmanager
def brain_context(brain: BrainWrapper, auto_initialize: bool = True) -> ContextManager:
    """
    Context manager for brain operations.
    
    Args:
        brain: Brain wrapper instance
        auto_initialize: Whether to automatically initialize the brain
        
    Yields:
        Brain wrapper
        
    Example:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        with brain_context(brain) as brain:
            for step in range(100):
                brain.step(step)
    """
    if auto_initialize:
        brain.initialize()
    try:
        yield brain
    finally:
        brain.reset()

# Convenience functions
def run_brain_simulation(
    config: Optional[pynlm.Config] = None,
    steps: int = 100,
    sensory_inputs: Optional[List[Any]] = None,
    auto_initialize: bool = True
) -> BrainWrapper:
    """
    Run a complete brain simulation with convenient API.
    
    Args:
        config: Brain configuration
        steps: Number of simulation steps
        sensory_inputs: List of sensory inputs (one per step)
        auto_initialize: Whether to automatically initialize the brain
        
    Returns:
        Brain wrapper instance
    """
    brain = create_brain(config)
    wrapper = BrainWrapper(brain)
    
    with brain_context(wrapper, auto_initialize):
        for step in range(steps):
            if sensory_inputs and step < len(sensory_inputs):
                wrapper.process_sensory_input(sensory_inputs[step])
            wrapper.step(step)
    
    return wrapper

def create_brain_with_config(config_dict: Dict[str, Any]) -> BrainWrapper:
    """
    Create a brain with configuration from a dictionary.
    
    Args:
        config_dict: Configuration dictionary
        
    Returns:
        Brain wrapper instance
    """
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    return BrainWrapper(pynlm.createBrain(config))

# Re-export enhanced functions and classes
__all__ = [
    'create_brain',
    'BrainWrapper',
    'brain_context',
    'run_brain_simulation',
    'create_brain_with_config',
]

# Try to export the original Brain class for advanced users
try:
    __all__.append('pynlm')
except NameError:
    pass
