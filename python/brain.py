# -*- coding: utf-8 -*-
"""
High-level Brain abstraction for NLM Python package.

This module provides a high-level interface for working with NLM brains,
abstracting away some of the low-level details and providing more Pythonic APIs.
"""

from typing import Optional, List, Dict, Any
import time

from .bindings import (
    Brain as _Brain,
    Config as _Config,
    createDefaultConfig,
    SimulationStep,
    Timestamp,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
)
class Brain:
    """
    High-level interface for NLM brain simulation.
    
    This class wraps the low-level C++ Brain class and provides a more
    Pythonic interface for common operations.
    """
    
    def __init__(self, config: Optional[_Config] = None):
        """
        Initialize a new brain.
        
        Args:
            config: Configuration for the brain. If None, a default
                   configuration will be created.
        """
        self._brain = _Brain(config) if config else _Brain(createDefaultConfig())
    
    def initialize(self):
        """Initialize the brain with its configuration."""
        self._brain.initialize()
        return self
    
    def step(self, step: SimulationStep, current_time: Optional[Timestamp] = None):
        """
        Perform a simulation step.
        
        Args:
            step: Current simulation step number
            current_time: Current simulation time (optional)
        """
        if current_time is not None:
            self._brain.step(step, current_time)
        else:
            self._brain.step(step)
        return self
    
    def get_neurons(self) -> List[Dict[str, Any]]:
        """
        Get information about all neurons in the brain.
        
        Returns:
            List of dictionaries with neuron information
        """
        # This would need to be implemented based on available C++ API
        return []
    
    def get_synapses(self) -> List[Dict[str, Any]]:
        """
        Get information about all synapses in the brain.
        
        Returns:
            List of dictionaries with synapse information
        """
        # This would need to be implemented based on available C++ API
        return []
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get current brain statistics.
        
        Returns:
            Dictionary with brain statistics
        """
        return {
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'developmental_stage': self._brain.getDevelopmentalStage(),
        }
    
    def __repr__(self):
        return f"<Brain with {self._brain.getTotalNeuronCount()} neurons>"


def createBrain(config: Optional[_Config] = None) -> Brain:
    """
    Create a new brain with optional configuration.
    
    Args:
        config: Configuration for the brain. If None, a default
               configuration will be created.
    
    Returns:
        New Brain instance
    """
    return Brain(config)


# For backward compatibility
Brain = createBrain
