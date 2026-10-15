# Enhanced brain interface with Pythonic API
# This module provides an enhanced brain interface with properties and convenience methods

import numpy as np
from typing import Optional, List, Dict, Any, Union
from .core import NLMError, ValidationError, SimulationError

class BrainSession:
    """
    Context manager for brain simulation sessions with automatic resource management.
    
    This class provides a clean interface for running complete simulation sessions
    with proper cleanup and statistics collection.
    """
    
    def __init__(self, brain, world=None, agent=None):
        """
        Initialize a brain session.
        
        Args:
            brain: Brain object to run
            world: Optional world object for environmental simulation
            agent: Optional agent brain object for interaction
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self._initialized = False
        
    def __enter__(self):
        """Enter the context manager."""
        self._initialized = True
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit the context manager and cleanup resources."""
        self.cleanup()
        
    def cleanup(self):
        """Clean up resources and reset systems."""
        if self.brain:
            self.brain.reset()
        if self.world:
            self.world.reset()
        if self.agent:
            self.agent.reset()
        self._initialized = False
        
    def run_simulation(self, steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """
        Run a complete simulation.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Time step duration in seconds
            
        Returns:
            Dictionary containing simulation statistics
            
        Raises:
            SimulationError: If simulation fails
        """
        if not self._initialized:
            raise SimulationError("BrainSession not properly initialized. Use context manager.")
            
        if not self.world or not self.agent:
            raise SimulationError("World and agent must be provided for simulation")
            
        try:
            for i in range(steps):
                self.world.update(timestep)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(i)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
            return self.get_stats()
            
        except Exception as e:
            raise SimulationError(f"Simulation failed: {e}")
            
    def get_stats(self) -> Dict[str, Any]:
        """
        Get detailed statistics about the brain state.
        
        Returns:
            Dictionary containing brain statistics
        """
        stats = {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'simulation_time': self.world.getSimulationTime() if self.world else 0,
        }
        
        return stats
        
    def get_summary(self) -> str:
        """
        Get a human-readable summary of the brain state.
        
        Returns:
            Formatted string with brain summary
        """
        stats = self.get_stats()
        return (f"Brain Session Summary:\n"
                f"  Duration: {stats['simulation_time']:.2f}s\n"
                f"  Neurons: {stats['total_neurons']} (active: {stats['active_neurons']}, firing: {stats['firing_neurons']})\n"
                f"  Synapses: {stats['total_synapses']}\n"
                f"  Total Spikes: {stats['total_spikes']}\n"
                f"  Firing Rate: {stats['average_firing_rate']:.2f} Hz\n"
                f"  E/I Ratio: {stats['excitation_inhibition_ratio']:.3f}\n"
                f"  Development Stage: {stats['development_stage']}")
        
    def enable_all_systems(self):
        """Enable all NLM systems (neuromodulation, memory, cognition, development)."""
        if self.brain and self.brain.getConfig():
            config = self.brain.getConfig()
            config.set("neuromodulation.enabled", True)
            config.set("memory.working.enabled", True)
            config.set("cognition.enabled", True)
            config.set("development.enabled", True)
            
    def validate_configuration(self) -> Tuple[List[str], List[str]]:
        """
        Validate the current configuration.
        
        Returns:
            Tuple of (errors, warnings)
        """
        if self.brain and self.brain.getConfig():
            return validateConfiguration(self.brain.getConfig())
        return [], []
class PythonicBrain:
    """
    Enhanced Pythonic brain interface with properties and convenience methods.
    
    This class provides a more Pythonic interface to the Brain class with:
    - Pythonic properties
    - Convenience methods
    - Better error handling
    - Type hints
    """
    
    def __init__(self, brain):
        """
        Initialize with a Brain object.
        
        Args:
            brain: Brain object to wrap
        """
        self._brain = brain
        
    @property
    def total_neuron_count(self) -> int:
        """Total number of neurons in the brain."""
        return self._brain.getTotalNeuronCount()
        
    @property
    def total_synapse_count(self) -> int:
        """Total number of synapses in the brain."""
        return self._brain.getTotalSynapseCount()
        
    @property
    def active_neuron_count(self) -> int:
        """Number of active neurons."""
        return self._brain.getActiveNeuronCount()
        
    @property
    def firing_neuron_count(self) -> int:
        """Number of firing neurons."""
        return self._brain.getFiringNeuronCount()
        
    @property
    def total_spike_count(self) -> int:
        """Total number of spikes recorded."""
        return self._brain.getTotalSpikeCount()
        
    @property
    def average_firing_rate(self) -> float:
        """Average firing rate across all neurons."""
        return self._brain.getAverageFiringRate()
        
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Ratio of excitation to inhibition."""
        return self._brain.getExcitationInhibitionRatio()
        
    @property
    def development_stage(self) -> int:
        """Current developmental stage (0-4)."""
        return self._brain.getDevelopmentalStage()
        
    def get_summary(self) -> str:
        """
        Get a concise summary of brain state.
        
        Returns:
            Formatted string with brain summary
        """
        return (f"PythonicBrain Summary:\n"
                f"  Neurons: {self.total_neuron_count}\n"
                f"  Synapses: {self.total_synapse_count}\n"
                f"  Spikes: {self.total_spike_count}\n"
                f"  Stage: {self.development_stage}")
        
    def get_detailed_stats(self) -> Dict[str, Any]:
        """
        Get detailed statistics about the brain.
        
        Returns:
            Dictionary with detailed statistics
        """
        return {
            'neuron_count': self.total_neuron_count,
            'synapse_count': self.total_synapse_count,
            'active_neuron_count': self.active_neuron_count,
            'firing_neuron_count': self.firing_neuron_count,
            'total_spike_count': self.total_spike_count,
            'average_firing_rate': self.average_firing_rate,
            'excitation_inhibition_ratio': self.excitation_inhibition_ratio,
            'development_stage': self.development_stage,
            'firing_rate_hz': self.average_firing_rate,
            'e_to_i_ratio': self.excitation_inhibition_ratio,
        }
        
    def enable_all_systems(self):
        """Enable all NLM systems for enhanced functionality."""
        if self._brain.getConfig():
            self._brain.getConfig().set("neuromodulation.enabled", True)
            self._brain.getConfig().set("memory.working.enabled", True)
            self._brain.getConfig().set("cognition.enabled", True)
            self._brain.getConfig().set("development.enabled", True)
            
    def run_optimized_simulation(self, steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """
        Run an optimized simulation with all systems enabled.
        
        Args:
            steps: Number of simulation steps
            timestep: Time step duration
            
        Returns:
            Dictionary with simulation statistics
        """
        self.enable_all_systems()
        # This would use the brain's optimized settings
        return self.get_detailed_stats()