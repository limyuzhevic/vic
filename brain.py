"""
Brain wrapper with enhanced Python API for NLM

This module provides a Pythonic interface to the C++ Brain class with
convenience methods, progress callbacks, and comprehensive statistics.
"""

import time
from typing import Optional, Callable, Dict, Any, List
from enum import Enum

# Import the C++ bindings
from .pynlm import (
    Brain as CppBrain,
    createBrain as create_cpp_brain,
    SimulationStep,
    Timestamp,
    RegionId,
    NeuronId,
    SynapseId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    Config,
    SensoryInput,
)

class ProgressCallback:
    """Progress callback handler for brain simulation."""
    
    def __init__(self, callback: Callable[[int, SimulationStep, Dict[str, Any]], None], 
                 interval: int = 100, min_step: int = 0):
        """Initialize progress callback.
        
        Args:
            callback: Function to call with progress updates
            interval: Call interval in simulation steps
            min_step: Minimum step number before first callback
        """
        self.callback = callback
        self.interval = interval
        self.min_step = min_step
        self.last_step = min_step - interval
    
    def should_call(self, step: SimulationStep) -> bool:
        """Check if callback should be called for this step.
        
        Args:
            step: Current simulation step
            
        Returns:
            True if callback should be called
        """
        return (step - self.min_step) % self.interval == 0 and step > self.last_step
    
    def call(self, step: SimulationStep, total_steps: SimulationStep = 0, 
             stats: Optional[Dict[str, Any]] = None):
        """Call the callback if conditions are met.
        
        Args:
            step: Current simulation step
            total_steps: Total simulation steps (for progress percentage)
            stats: Current simulation statistics
        """
        if self.should_call(step):
            progress = (step / total_steps * 100) if total_steps > 0 else 0
            self.callback(step, progress, stats or {})
            self.last_step = step
class Brain:
    """Brain simulation with enhanced Python API."""
    
    def __init__(self, config: Optional[Config] = None):
        """Create brain with optional configuration.
        
        Args:
            config: Configuration object
        """
        if config is None:
            from .config import create_default_config
            config = create_default_config()
        
        self._cpp_brain = create_cpp_brain(config)
        self._progress_callbacks: List[ProgressCallback] = []
        self._stats_history: List[Dict[str, Any]] = []
        self._start_time = time.time()
    
    def initialize(self):
        """Initialize the brain with configuration."""
        self._cpp_brain.initialize()
    
    def step(self, step: SimulationStep, current_time: Optional[Timestamp] = None):
        """Perform a simulation step with progress callbacks.
        
        Args:
            step: Current simulation step number
            current_time: Current simulation time in seconds
        """
        # Get stats before step
        stats = self.get_stats()
        
        # Call progress callbacks
        for callback in self._progress_callbacks:
            callback.call(step, 0, stats)
        
        # Perform simulation step
        if current_time is not None:
            self._cpp_brain.step(step, current_time)
        else:
            self._cpp_brain.step(step)
        
        # Record stats after step
        self._stats_history.append(self.get_stats())
    
    def add_progress_callback(self, callback: Callable[[int, float, Dict[str, Any]], None], 
                              interval: int = 100, min_step: int = 0):
        """Add a progress callback for long-running simulations.
        
        Args:
            callback: Function to call with (step, progress_percent, stats)
            interval: Call interval in simulation steps
            min_step: Minimum step number before first callback
        """
        self._progress_callbacks.append(ProgressCallback(callback, interval, min_step))
    
    def remove_progress_callback(self, callback: Callable[[int, float, Dict[str, Any]], None]):
        """Remove a progress callback.
        
        Args:
            callback: Callback function to remove
        """
        self._progress_callbacks = [cb for cb in self._progress_callbacks 
                                   if cb.callback != callback]
    
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive simulation statistics.
        
        Returns:
            Dictionary with brain statistics
        """
        cpp_stats = {
            'total_spike_count': self._cpp_brain.getTotalSpikeCount(),
            'firing_neuron_count': self._cpp_brain.getFiringNeuronCount(),
            'total_neuron_count': self._cpp_brain.getTotalNeuronCount(),
            'total_synapse_count': self._cpp_brain.getTotalSynapseCount(),
            'active_neuron_count': self._cpp_brain.getActiveNeuronCount(),
            'average_firing_rate': self._cpp_brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self._cpp_brain.getExcitationInhibitionRatio(),
            'region_count': self._cpp_brain.getRegionCount(),
            'developmental_stage': self._cpp_brain.getDevelopmentalStage(),
        }
        
        # Add computed statistics
        elapsed_time = time.time() - self._start_time
        
        # Calculate firing frequency (spikes per second)
        firing_frequency = cpp_stats['total_spike_count'] / elapsed_time if elapsed_time > 0 else 0
        
        # Calculate active neuron percentage
        active_percentage = (cpp_stats['active_neuron_count'] / max(cpp_stats['total_neuron_count'], 1)) * 100
        
        cpp_stats.update({
            'elapsed_time': elapsed_time,
            'firing_frequency': firing_frequency,
            'active_neuron_percentage': active_percentage,
        })
        
        return cpp_stats
    
    def run_simulation(self, steps: SimulationStep, callback: Optional[Callable[[int, float, Dict[str, Any]], None]] = None, 
                      interval: int = 100, start_time: Optional[Timestamp] = None):
        """Run simulation with optional progress callback.
        
        Args:
            steps: Total number of simulation steps to run
            callback: Optional progress callback function
            interval: Progress callback interval in steps
            start_time: Optional start time for simulation
        """
        if callback is not None:
            self.add_progress_callback(callback, interval)
        
        current_time = start_time or 0.0
        for step in range(steps):
            self.step(step, current_time)
            current_time += 0.1  # Default timestep
        
        if callback is not None:
            self.remove_progress_callback(callback)
    
    def save_state(self, filepath: str):
        """Save brain state to file.
        
        Args:
            filepath: Path to save brain state
        """
        self._cpp_brain.save(filepath)
    
    def load_state(self, filepath: str):
        """Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
        """
        self._cpp_brain.load(filepath)
        self._start_time = time.time()
    
    def receive_sensory_input(self, input_data: SensoryInput):
        """Inject sensory input into the brain.
        
        Args:
            input_data: Sensory input data
        """
        self._cpp_brain.receiveSensoryInput(input_data)
    
    def inject_current(self, neuron: NeuronId, current: float):
        """Inject current into a specific neuron.
        
        Args:
            neuron: Neuron ID
            current: Current to inject (microsiemens)
        """
        self._cpp_brain.injectCurrent(neuron, current)
    
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float):
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to target
            current: Current to inject (microsiemens)
        """
        self._cpp_brain.injectCurrentToNeurons(neuron_type, current)
    
    def produce_action(self):
        """Produce motor action based on neural activity.
        
        Returns:
            Action produced by the brain
        """
        from .pynlm import Action
        return self._cpp_brain.produceAction()
    
    def reset(self):
        """Reset brain state."""
        self._cpp_brain.reset()
        self._stats_history.clear()
        self._start_time = time.time()
    
    def add_region(self, name: str = "") -> RegionId:
        """Add a new neural region.
        
        Args:
            name: Optional region name
            
        Returns:
            Region ID of the newly created region
        """
        return self._cpp_brain.addRegion(name)
    
    def get_region(self, region_id: RegionId):
        """Get a region by ID.
        
        Args:
            region_id: Region ID
            
        Returns:
            Region object
        """
        return self._cpp_brain.getRegion(region_id)
    
    def get_region_count(self) -> int:
        """Get the number of regions.
        
        Returns:
            Number of regions
        """
        return self._cpp_brain.getRegionCount()
    
    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs.
        
        Returns:
            List of region IDs
        """
        return self._cpp_brain.getRegionIds()
    
    def get_regions(self) -> List[Any]:
        """Get all regions.
        
        Returns:
            List of region objects
        """
        return self._cpp_brain.getRegions()
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count across all regions.
        
        Returns:
            Total neuron count
        """
        return self._cpp_brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count across all regions.
        
        Returns:
            Total synapse count
        """
        return self._cpp_brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons.
        
        Returns:
            Count of active neurons
        """
        return self._cpp_brain.getActiveNeuronCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons.
        
        Returns:
            Count of firing neurons
        """
        return self._cpp_brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons.
        
        Returns:
            Average firing rate (Hz)
        """
        return self._cpp_brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio.
        
        Returns:
            Excitation/inhibition ratio
        """
        return self._cpp_brain.getExcitationInhibitionRatio()
    
    def get_total_spike_count(self) -> SimulationStep:
        """Get total spike count.
        
        Returns:
            Total spike count
        """
        return self._cpp_brain.getTotalSpikeCount()
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage
        """
        return self._cpp_brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage: DevelopmentalStage):
        """Set developmental stage.
        
        Args:
            stage: New developmental stage
        """
        self._cpp_brain.setDevelopmentalStage(stage)
    
    def get_config(self) -> Config:
        """Get the configuration.
        
        Returns:
            Configuration object
        """
        return self._cpp_brain.getConfig()
    
    def log_status(self):
        """Log brain status to the global logger."""
        self._cpp_brain.logStatus()
    
    def get_stats_history(self) -> List[Dict[str, Any]]:
        """Get the history of recorded statistics.
        
        Returns:
            List of statistics dictionaries
        """
        return self._stats_history.copy()
    
    def clear_stats_history(self):
        """Clear the statistics history."""
        self._stats_history.clear()
    
    def __repr__(self) -> str:
        """Get string representation of brain.
        
        Returns:
            String representation
        """
        stats = self.get_stats()
        return (f"Brain(neurons={stats['total_neuron_count']}, "
                f"spikes={stats['total_spike_count']}, "
                f"firing={stats['firing_neuron_count']}, "
                f"regions={stats['region_count']}, "
                f"stage={stats['developmental_stage']})")

# Convenience function

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a brain with optional configuration.
    
    Args:
        config: Configuration object
        
    Returns:
        Brain instance
    """
    return Brain(config)

# Export types and classes
__all__ = [
    'Brain',
    'ProgressCallback',
    'create_brain',
    'SimulationStep',
    'Timestamp',
    'RegionId',
    'NeuronId',
    'SynapseId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
]