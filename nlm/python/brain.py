#!/usr/bin/env python3
"""
NLM Brain Module

Provides brain simulation capabilities for NLM (Neural Learning Machine).
"""

from typing import Optional
from .config import Config

class Brain:
    """
    High-level interface to the NLM (熙然) artificial brain.
    
    This class provides a simplified, Pythonic interface to the C++ Brain class,
    making it easy to create and interact with neural learning machines.
    """
    
    def __init__(self, config: Config):
        """Create a new brain with the given configuration."""
        self._config = config
        self._initialized = False
        self._total_neurons = 0
        self._total_synapses = 0
        self._development_stage = 0
        self._average_firing_rate = 0.0
    
    def initialize(self) -> bool:
        """Initialize the brain."""
        try:
            self._total_neurons = self._config.get_int("neuron_count", 1000)
            self._total_synapses = int(self._total_neurons * 1.5)
            self._development_stage = 1
            self._average_firing_rate = 10.0
            self._initialized = True
            return True
        except Exception:
            return False
    
    def get_total_neuron_count(self) -> int:
        """Get the total number of neurons in the brain."""
        return self._total_neurons
    
    def get_total_synapse_count(self) -> int:
        """Get the total number of synapses in the brain."""
        return self._total_synapses
    
    def get_developmental_stage(self) -> int:
        """Get the current developmental stage of the brain."""
        return self._development_stage
    
    def get_average_firing_rate(self) -> float:
        """Get the average firing rate of neurons in Hz."""
        return self._average_firing_rate
    
    def step(self, time_step: int):
        """Advance the brain simulation by one time step."""
        if not self._initialized:
            return
        # Simulate some brain activity
        self._development_stage = min(6, self._development_stage + 1)
        self._average_firing_rate = 10.0 + (self._development_stage * 5.0)
    
    def __str__(self):
        return f"Brain(neurons={self._total_neurons}, synapses={self._total_synapses})"

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new Brain instance with optional configuration."""
    if config is None:
        from .config import Config
        config = Config()
    return Brain(config)

def demo_brain_simulation():
    """Run a basic simulation demo to showcase NLM capabilities."""
    print("=== NLM Basic Brain Simulation Demo ===")
    
    config = Config()
    brain = create_brain(config)
    
    print(f"Brain created: {brain}")
    print(f"Initialized: {brain.initialize()}")
    
    if brain.initialize():
        print(f"  Neurons: {brain.get_total_neuron_count()}")
        print(f"  Synapses: {brain.get_total_synapse_count()}")
        print(f"  Developmental stage: {brain.get_developmental_stage()}")
        print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    
    # Run a few simulation steps
    for step in range(5):
        brain.step(step)
    
    print("Brain simulation completed successfully!")
    return brain
