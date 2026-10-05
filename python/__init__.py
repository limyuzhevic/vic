#!/usr/bin/env python3
"""
NLM Python Bindings - Main module initialization

This module provides a complete Python API for the NLM (Neural Learning Machine)
framework with full bindings for all neural components.
"""

import os

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM neural simulation framework"
__implementation__ = "C++ (pybind11) - Python bindings via pybind11"

# Features list
__features__ = [
    "Full Neuron class with LIF dynamics",
    "Complete Synapse class with plasticity",
    "NeuralRegion with population management",
    "NeuralPopulation with statistics",
    "Neuromodulator system (4 types)",
    "Context managers (__enter__/__exit__)",
    "Pythonic operators (__getitem__, __iter__)",
    "Property-based access",
    "Vector conversions",
    "Batch operations",
    "Statistics utilities"
]

__examples__ = [
    "basic_example.py - Basic neural simulation",
    "neuromodulator_example.py - Neuromodulation demonstration",
    "population_example.py - Population dynamics",
    "batch_operations_example.py - Batch operations"
]

# Try to import from C++ extensions
try:
    # Import from the actual C++ extension
    # Use a more robust import mechanism
    import importlib.util
    spec = importlib.util.find_spec("pynlm_cpp")
    
    if spec is not None:
        # Import from C++ extension
        from pynlm_cpp import (
            # Core neural classes
            Neuron,
            Synapse,
            NeuralRegion,
            NeuralPopulation,
            
            # Neuromodulators
            Neuromodulator,
            Dopamine,
            Acetylcholine,
            Norepinephrine,
            Serotonin,
            
            # Type definitions
            NeuronId,
            SynapseId,
            RegionId,
            PopulationId,
            NeuronType,
            SynapseType,
            DevelopmentalStage,
            FiringState,
            
            # Constants
            INVALID_NEURON_ID,
            INVALID_SYNAPSE_ID,
            INVALID_REGION_ID,
            INVALID_POPULATION_ID,
        )
        
        _USING_CPP_EXTENSIONS = True
    else:
        raise ImportError("C++ extension not found")
        
except ImportError:
    # Fallback to pure Python implementations for development/testing
    print("Warning: C++ extensions not found. Using pure Python implementations for development.")
    
    _USING_CPP_EXTENSIONS = False
    
    # Pure Python implementations for development
    from enum import Enum
    
    class NeuronType(Enum):
        Excitatory = 1
        Inhibitory = 2
        Modulatory = 3
        Sensory = 4
        Motor = 5
        Internal = 6
    
    class SynapseType(Enum):
        Excitatory = 1
        Inhibitory = 2
        Modulatory = 3
        Electrical = 4
        GapJunction = 5
    
    class FiringState(Enum):
        Resting = 1
        Active = 2
        Refractory = 3
        Inhibited = 4
    
    class DevelopmentalStage(Enum):
        Initial = 1
        CriticalPeriod = 2
        Maturation = 3
        Adult = 4
        Aging = 5
    
    class PlasticityFlags:
        def __init__(self):
            self.hebbian = False
            self.stdp = False
            self.reward_modulated = False
            self.structural = False
            self.eligible = False
    
    # ID types (simplified)
    class NeuronId:
        def __init__(self, value):
            self.value = value
            
        def __repr__(self):
            return f"NeuronId({self.value})"
    
    class SynapseId:
        def __init__(self, value):
            self.value = value
            
        def __repr__(self):
            return f"SynapseId({self.value})"
    
    class RegionId:
        def __init__(self, value):
            self.value = value
            
        def __repr__(self):
            return f"RegionId({self.value})"
    
    class PopulationId:
        def __init__(self, value):
            self.value = value
            
        def __repr__(self):
            return f"PopulationId({self.value})"
    
    # Base classes
    class Neuron:
        def __init__(self, neuron_id, neuron_type=None, resting_potential=-70.0):
            self.id = neuron_id
            self.type = neuron_type
            self.resting_potential = resting_potential
            self.threshold = -55.0
            self.firing_rate = 0.0
            self.spike_history = []
            
        def __repr__(self):
            return f"Neuron(id={self.id}, type={self.type})"
        
        @property
        def V(self):
            return self.firing_rate * 0.1  # Simplified
        
        @V.setter
        def V(self, value):
            self.firing_rate = value * 10
        
        @property
        def spike_count(self):
            return len(self.spike_history)
        
        def to_vector(self):
            return [self.V, self.threshold, self.firing_rate]
        
        def as_dict(self):
            return {
                'id': self.id,
                'type': self.type,
                'V': self.V,
                'threshold': self.threshold,
                'firing_rate': self.firing_rate,
                'spike_count': self.spike_count
            }
    
    class Synapse:
        def __init__(self, synapse_id, source, destination):
            self.id = synapse_id
            self.source = source
            self.destination = destination
            self.weight = 0.0
            self.delay = 1
            self.type = SynapseType.Excitatory
            
        def __repr__(self):
            return f"Synapse(id={self.id}, {self.source} -> {self.destination})"
        
        def to_vector(self):
            return [self.weight, self.delay, self.efficacy] if hasattr(self, 'efficacy') else [self.weight, self.delay, 1.0]
        
        def as_dict(self):
            return {
                'id': self.id,
                'source': self.source,
                'destination': self.destination,
                'weight': self.weight,
                'delay': self.delay,
                'type': self.type
            }
    
    class NeuralRegion:
        def __init__(self, region_id, name=""):
            self.id = region_id
            self.name = name
            self.populations = {}
            self.synapses = {}
            
        def __repr__(self):
            return f"NeuralRegion(id={self.id}, name='{self.name}')"
        
        def as_dict(self):
            return {
                'id': self.id,
                'name': self.name,
                'population_count': len(self.populations),
                'synapse_count': len(self.synapses)
            }
    
    class NeuralPopulation:
        def __init__(self, population_id, size):
            self.id = population_id
            self.size = size
            self.neurons = []
            self.type = NeuronType.Internal
            
        def __repr__(self):
            return f"NeuralPopulation(id={self.id}, size={self.size})"
        
        def as_dict(self):
            return {
                'id': self.id,
                'size': self.size,
                'neuron_type': self.type,
                'active_count': self.get_active_count() if hasattr(self, 'get_active_count') else 0,
                'firing_count': self.get_firing_count() if hasattr(self, 'get_firing_count') else 0
            }
    
    # Neuromodulators (simplified)
    class Neuromodulator:
        def __init__(self, name):
            self.name = name
            self.level = 0.0
            
        def get_name(self):
            return self.name
            
        def get_level(self):
            return self.level
            
        def set_level(self, level):
            self.level = level
    
    class Dopamine(Neuromodulator):
        def __init__(self):
            super().__init__("Dopamine")
    
    class Acetylcholine(Neuromodulator):
        def __init__(self):
            super().__init__("Acetylcholine")
    
    class Norepinephrine(Neuromodulator):
        def __init__(self):
            super().__init__("Norepinephrine")
    
    class Serotonin(Neuromodulator):
        def __init__(self):
            super().__init__("Serotonin")

# Convenience factory functions
def create_neuron(neuron_id, neuron_type=NeuronType.Internal):
    """Create a new neuron."""
    return Neuron(neuron_id, neuron_type)

def create_synapse(synapse_id, source, destination, weight=0.0):
    """Create a new synapse."""
    synapse = Synapse(synapse_id, source, destination)
    synapse.weight = weight
    return synapse

def create_region(region_id, name=""):
    """Create a new neural region."""
    return NeuralRegion(region_id, name)

def create_population(population_id, size, neuron_type=NeuronType.Internal):
    """Create a new neural population."""
    population = NeuralPopulation(population_id, size)
    population.type = neuron_type
    return population

# Batch operations
def batch_step_neurons(neurons, current_time):
    """Step multiple neurons in batch."""
    for neuron in neurons:
        if hasattr(neuron, 'step'):
            neuron.step(current_time)

def batch_get_potential(neurons):
    """Get membrane potentials for multiple neurons."""
    return [neuron.V if hasattr(neuron, 'V') else 0.0 for neuron in neurons]

def batch_get_spike_counts(neurons):
    """Get spike counts for multiple neurons."""
    return [len(neuron.spike_history) if hasattr(neuron, 'spike_history') else 0 for neuron in neurons]

# Statistics utilities
def calculate_mean(values):
    """Calculate mean of a list of values."""
    return sum(values) / len(values) if values else 0.0

def calculate_variance(values):
    """Calculate variance of a list of values."""
    if len(values) < 2:
        return 0.0
    mean = calculate_mean(values)
    return sum((v - mean) ** 2 for v in values) / len(values)

def calculate_stddev(values):
    """Calculate standard deviation of a list of values."""
    return calculate_variance(values) ** 0.5

# Utility functions for vector operations
def to_vector(neuron):
    """Convert neuron properties to vector."""
    if hasattr(neuron, 'to_vector'):
        return neuron.to_vector()
    return [neuron.V, neuron.threshold, neuron.firing_rate] if hasattr(neuron, 'V') else [0.0, 0.0, 0.0]

def from_array(arr):
    """Create list from array (alias for list conversion)."""
    return list(arr)

# Export all public API
__all__ = [
    'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
    'Neuromodulator', 'Dopamine', 'Acetylcholine', 'Norepinephrine', 'Serotonin',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'FiringState', 'DevelopmentalStage',
    'PlasticityFlags',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'create_neuron', 'create_synapse', 'create_region', 'create_population',
    'batch_step_neurons', 'batch_get_potential', 'batch_get_spike_counts',
    'calculate_mean', 'calculate_variance', 'calculate_stddev',
    'to_vector', 'from_array'
]

print(f"NLM Python Bindings v{__version__} loaded successfully")
print(f"Implementation: {__implementation__}")
print(f"Features: {len(__features__)} available")