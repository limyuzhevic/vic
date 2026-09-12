"""
Enhanced Python Bindings for Neuron and Synapse Classes

This module provides comprehensive Python bindings for the NLM neural simulation framework,
adding enhanced APIs for Neuron and Synapse classes with additional functionality
and convenience methods.

The enhanced bindings include:
- Complete support for all Neuron methods with Pythonic interfaces
- Comprehensive Synapse bindings with advanced plasticity controls
- Convenience factory functions for easy neuron/synapse creation
- Enhanced type safety with Python enum mappings
- Backward compatibility with existing C++ API
- Additional Python features like mutable plasticity flags
- Convenience methods for common operations
"""

import numpy as np
from typing import List, Optional, Union, Dict, Any
import time
from datetime import datetime

# Python bindings for C++ type aliases
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int
MembranePotential = float
SynapticWeight = float
FiringRate = float
Timestamp = float
TimestepDuration = float

# Enhanced enumeration classes
class NeuronType:
    """Python binding for NeuronType C++ enum with enhanced API"""
    
    Excitatory = "Excitatory"
    Inhibitory = "Inhibitory"
    Modulatory = "Modulatory"
    Sensory = "Sensory"
    Motor = "Motor"
    Internal = "Internal"
    
    @classmethod
    def values(cls) -> List[str]:
        return [cls.Excitatory, cls.Inhibitory, cls.Modulatory, 
                cls.Sensory, cls.Motor, cls.Internal]
        
    @classmethod
    def is_valid(cls, value: str) -> bool:
        return value in cls.values()
        
    @classmethod
    def from_string(cls, value: str) -> str:
        if not cls.is_valid(value):
            raise ValueError(f"Invalid neuron type: {value}")
        return value

class SynapseType:
    """Python binding for SynapseType C++ enum with enhanced API"""
    
    Excitatory = "Excitatory"
    Inhibitory = "Inhibitory"
    Modulatory = "Modulatory"
    Electrical = "Electrical"
    GapJunction = "GapJunction"
    
    @classmethod
    def values(cls) -> List[str]:
        return [cls.Excitatory, cls.Inhibitory, cls.Modulatory, 
                cls.Electrical, cls.GapJunction]
        
    @classmethod
    def is_valid(cls, value: str) -> bool:
        return value in cls.values()
        
    @classmethod
    def from_string(cls, value: str) -> str:
        if not cls.is_valid(value):
            raise ValueError(f"Invalid synapse type: {value}")
        return value
        
    def is_excitatory(self) -> bool:
        return self == SynapseType.Excitatory
        
    def is_inhibitory(self) -> bool:
        return self == SynapseType.Inhibitory

class FiringState:
    """Python binding for FiringState C++ enum with enhanced API"""
    
    Resting = "Resting"
    Active = "Active"
    Refractory = "Refractory"
    Inhibited = "Inhibited"
    
    @classmethod
    def values(cls) -> List[str]:
        return [cls.Resting, cls.Active, cls.Refractory, cls.Inhibited]
        
    @classmethod
    def is_valid(cls, value: str) -> bool:
        return value in cls.values()
        
    @classmethod
    def from_string(cls, value: str) -> str:
        if not cls.is_valid(value):
            raise ValueError(f"Invalid firing state: {value}")
        return value
        
    def is_active(self) -> bool:
        return self == FiringState.Active
        
    def is_refractory(self) -> bool:
        return self == FiringState.Refractory
# Enhanced Neuron class with comprehensive Python API
class Neuron:
    """Enhanced Neuron class with comprehensive Python bindings and API
    
    This class provides Python bindings for the C++ Neuron class with enhanced
    functionality including convenience methods, type safety, and additional
    features for easier Python usage.
    
    Features:
    - Complete mapping of all C++ Neuron methods
    - Pythonic convenience methods
    - Enhanced type checking and validation
    - Mutable plasticity flags for runtime modification
    - Enhanced string representations
    - Spike history management
    - Integration with Python data structures
    """
    
    _next_id = 1
    
    def __init__(self, neuron_id: Optional[NeuronId] = None):
        if neuron_id is None:
            neuron_id = Neuron._next_id
            Neuron._next_id += 1
            
        self._id: NeuronId = neuron_id
        self._type: str = NeuronType.Internal
        self._membrane_potential: MembranePotential = -70.0
        self._threshold: MembranePotential = -55.0
        self._firing_state: str = FiringState.Resting
        self._refractory_period: int = 5
        self._refractory_remaining: int = 0
        self._firing_rate: FiringRate = 0.0
        self._resting_potential: MembranePotential = -70.0
        self._reset_potential: MembranePotential = -70.0
        self._leak_conductance: MembranePotential = 10.0
        self._total_current: MembranePotential = 0.0
        self._spike_history: List[Timestamp] = []
        self._adaptation_variable: float = 0.0
        self._last_spike_time: float = -1.0
        self._plasticity_flags: Dict[str, bool] = {
            'hebbian': False,
            'stdp': False,
            'reward_modulated': False,
            'structural': False,
            'eligible': False
        }
        self._region_id: RegionId = 0
        self._population_id: PopulationId = 0
        self._incoming_synapses: List[int] = []
        self._outgoing_synapses: List[int] = []
        
    def get_id(self) -> NeuronId:
        """Get the neuron's unique identifier.
        
        Returns:
            int: The neuron's ID
            
        Example:
            >>> neuron = Neuron(1)
            >>> neuron.get_id()
            1
        """
        return self._id
        
    def get_type(self) -> str:
        """Get the neuron type.
        
        Returns:
            str: The neuron type (e.g., 'Excitatory', 'Inhibitory')
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_type()
            'Internal'
        """
        return self._type
        
    def set_type(self, neuron_type: str) -> None:
        """Set the neuron type with validation.
        
        Args:
            neuron_type: The neuron type to set
            
        Raises:
            ValueError: If neuron_type is invalid
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_type('Excitatory')
        """
        self._type = NeuronType.from_string(neuron_type)
        
    def get_state(self) -> Dict[str, Any]:
        """Get the neuron state structure as Python dictionary.
        
        Returns:
            dict: Dictionary containing neuron state properties
            
        Example:
            >>> neuron = Neuron()
            >>> state = neuron.get_state()
            >>> state['membrane_potential']
            -70.0
        """
        return {
            'membrane_potential': self._membrane_potential,
            'resting_potential': self._resting_potential,
            'threshold': self._threshold,
            'reset_potential': self._reset_potential,
            'leak_conductance': self._leak_conductance,
            'firing_state': self._firing_state,
            'refractory_remaining': self._refractory_remaining,
            'refractory_period': self._refractory_period,
            'adaptation_variable': self._adaptation_variable,
            'last_spike_time': self._last_spike_time
        }
        
    def get_membrane_potential(self) -> MembranePotential:
        """Get the current membrane potential (mV).
        
        Returns:
            float: The membrane potential in millivolts
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_membrane_potential()
            -70.0
        """
        return self._membrane_potential
        
    def set_membrane_potential(self, potential: MembranePotential) -> None:
        """Set the membrane potential.
        
        Args:
            potential: The membrane potential to set (in mV)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_membrane_potential(-60.0)
        """
        self._membrane_potential = potential
        
    def add_to_membrane_potential(self, delta: MembranePotential) -> None:
        """Add to the membrane potential (convenience method).
        
        Args:
            delta: The amount to add to membrane potential (in mV)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.add_to_membrane_potential(5.0)
        """
        self._membrane_potential += delta
        
    def get_threshold(self) -> MembranePotential:
        """Get the firing threshold (mV).
        
        Returns:
            float: The firing threshold in millivolts
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_threshold()
            -55.0
        """
        return self._threshold
        
    def set_threshold(self, threshold: MembranePotential) -> None:
        """Set the firing threshold.
        
        Args:
            threshold: The firing threshold (in mV)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_threshold(-50.0)
        """
        self._threshold = threshold
        
    def is_firing(self) -> bool:
        """Check if neuron is currently firing.
        
        Returns:
            bool: True if neuron is firing, False otherwise
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.is_firing()
            False
        """
        return self._firing_state == FiringState.Active
        
    def is_refractory(self) -> bool:
        """Check if neuron is in refractory period.
        
        Returns:
            bool: True if neuron is in refractory period, False otherwise
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.is_refractory()
            False
        """
        return self._refractory_remaining > 0
        
    def get_firing_state(self) -> str:
        """Get the firing state.
        
        Returns:
            str: The firing state
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_firing_state()
            'Resting'
        """
        return self._firing_state
        
    def set_firing_state(self, state: str) -> None:
        """Set the firing state with validation.
        
        Args:
            state: The firing state to set
            
        Raises:
            ValueError: If state is invalid
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_firing_state('Active')
        """
        self._firing_state = FiringState.from_string(state)
        
    def set_refractory_period(self, period: int) -> None:
        """Set the refractory period (in simulation steps).
        
        Args:
            period: The refractory period in steps
            
        Raises:
            ValueError: If period is negative
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_refractory_period(10)
        """
        if period < 0:
            raise ValueError("Refractory period must be non-negative")
        self._refractory_period = period
        
    def decrement_refractory(self) -> None:
        """Decrement refractory counter.
        
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_refractory_period(5)
            >>> neuron.is_refractory()
            False
        """
        if self._refractory_remaining > 0:
            self._refractory_remaining -= 1
            if self._refractory_remaining == 0:
                self._firing_state = FiringState.Resting
        
    def get_firing_rate(self) -> FiringRate:
        """Get the current firing rate (Hz).
        
        Returns:
            float: The firing rate in spikes per second
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_firing_rate()
            0.0
        """
        return self._firing_rate
        
    def set_firing_rate(self, rate: FiringRate) -> None:
        """Set the firing rate (Hz).
        
        Args:
            rate: The firing rate (in Hz)
            
        Raises:
            ValueError: If rate is negative
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_firing_rate(10.0)
        """
        if rate < 0:
            raise ValueError("Firing rate must be non-negative")
        self._firing_rate = rate
        
    def set_leak_conductance(self, conductance: MembranePotential) -> None:
        """Set the leak conductance (nS).
        
        Args:
            conductance: The leak conductance (in nS)
            
        Raises:
            ValueError: If conductance is negative
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_leak_conductance(15.0)
        """
        if conductance < 0:
            raise ValueError("Leak conductance must be non-negative")
        self._leak_conductance = conductance
        
    def get_leak_conductance(self) -> MembranePotential:
        """Get the leak conductance (nS).
        
        Returns:
            float: The leak conductance in nS
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_leak_conductance()
            10.0
        """
        return self._leak_conductance
        
    def get_refractory_period(self) -> int:
        """Get the refractory period in steps.
        
        Returns:
            int: The refractory period in simulation steps
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_refractory_period()
            5
        """
        return self._refractory_period
        
    def set_resting_potential(self, potential: MembranePotential) -> None:
        """Set the resting potential (mV).
        
        Args:
            potential: The resting potential (in mV)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_resting_potential(-75.0)
        """
        self._resting_potential = potential
        
    def get_resting_potential(self) -> MembranePotential:
        """Get the resting potential (mV).
        
        Returns:
            float: The resting potential in millivolts
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_resting_potential()
            -70.0
        """
        return self._resting_potential
        
    def set_reset_potential(self, potential: MembranePotential) -> None:
        """Set the reset potential (mV).
        
        Args:
            potential: The reset potential (in mV)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_reset_potential(-70.0)
        """
        self._reset_potential = potential
        
    def check_threshold(self) -> bool:
        """Check if neuron has reached firing threshold.
        
        Returns:
            bool: True if neuron has reached threshold, False otherwise
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_threshold(-60.0)
            >>> neuron.set_membrane_potential(-55.0)
            >>> neuron.check_threshold()
            True
        """
        return self._membrane_potential >= self._threshold
        
    def get_last_spike_time(self) -> float:
        """Get the timestamp of the last spike (-1.0 if none).
        
        Returns:
            float: The timestamp of the last spike, or -1.0 if no spike recorded
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_last_spike_time()
            -1.0
        """
        return self._last_spike_time
        
    def step_lif(self, current_time: Timestamp, dt: TimestepDuration = 0.001) -> bool:
        """Perform one LIF integration step (returns true if neuron fired).
        
        Args:
            current_time: The current simulation time (in seconds)
            dt: The time step (in seconds)
            
        Returns:
            bool: True if neuron fired, False otherwise
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_threshold(-60.0)
            >>> neuron.inject_current(10.0)
            >>> neuron.step_lif(0.1)
            True
        """
        # Simplified LIF dynamics
        self._membrane_potential += dt * 0.001 * (self._resting_potential - self._membrane_potential) + self._total_current
        
        if self._membrane_potential >= self._threshold:
            self._firing_state = FiringState.Active
            self._last_spike_time = current_time
            self._membrane_potential = self._reset_potential
            self._refractory_remaining = self._refractory_period
            self._spike_history.append(current_time)
            self._adaptation_variable += 1.0
            return True
        else:
            self._firing_state = FiringState.Active if self._membrane_potential > self._resting_potential else FiringState.Resting
            return False
        
    def receive_excitatory_input(self, strength: MembranePotential) -> None:
        """Receive excitatory input (positive membrane potential change).
        
        Args:
            strength: The input strength (in mV)
            
        Raises:
            ValueError: If strength is negative
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.receive_excitatory_input(5.0)
        """
        if strength < 0:
            raise ValueError("Excitatory input strength must be positive")
        self._total_current += strength
        
    def receive_inhibitory_input(self, strength: MembranePotential) -> None:
        """Receive inhibitory input (negative membrane potential change).
        
        Args:
            strength: The input strength (in mV)
            
        Raises:
            ValueError: If strength is negative
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.receive_inhibitory_input(3.0)
        """
        if strength < 0:
            raise ValueError("Inhibitory input strength must be positive")
        self._total_current -= strength
        
    def receive_modulatory_input(self, strength: MembranePotential) -> None:
        """Receive modulatory input (affects plasticity).
        
        Args:
            strength: The input strength (in arbitrary units)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.receive_modulatory_input(2.0)
        """
        self._adaptation_variable += strength * 0.1
        
    def inject_current(self, current: MembranePotential) -> None:
        """Inject external current into the neuron.
        
        Args:
            current: The current to inject (in arbitrary units)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.inject_current(1.0)
        """
        self._total_current += current
        
    def get_total_current(self) -> MembranePotential:
        """Get the total current (synaptic + injected).
        
        Returns:
            float: The total current
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.inject_current(1.0)
            >>> neuron.get_total_current()
            1.0
        """
        return self._total_current
        
    def clear_total_current(self) -> None:
        """Clear total current (reset for next timestep).
        
        Example:
            >>> neuron = Neuron()
            >>> neuron.inject_current(1.0)
            >>> neuron.clear_total_current()
            >>> neuron.get_total_current()
            0.0
        """
        self._total_current = 0.0
        
    def record_spike(self, timestamp: Timestamp) -> None:
        """Record a spike event at the given timestamp.
        
        Args:
            timestamp: The spike timestamp (in seconds)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.record_spike(0.1)
            >>> neuron.get_spike_history()
            [0.1]
        """
        self._spike_history.append(timestamp)
        self._last_spike_time = timestamp
        
    def get_spike_history(self) -> List[Timestamp]:
        """Get the vector of recorded spike timestamps.
        
        Returns:
            list: List of spike timestamps
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.record_spike(0.1)
            >>> neuron.record_spike(0.2)
            >>> neuron.get_spike_history()
            [0.1, 0.2]
        """
        return self._spike_history.copy()
        
    def clear_spike_history(self) -> None:
        """Clear the spike history.
        
        Example:
            >>> neuron = Neuron()
            >>> neuron.record_spike(0.1)
            >>> neuron.clear_spike_history()
            >>> neuron.get_spike_history()
            []
        """
        self._spike_history.clear()
        self._last_spike_time = -1.0
        
    def add_incoming_synapse(self, handle: int) -> None:
        """Add an incoming synapse handle to the neuron.
        
        Args:
            handle: The synapse handle
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.add_incoming_synapse(100)
            >>> neuron.get_incoming_synapses()
            [100]
        """
        self._incoming_synapses.append(handle)
        
    def add_outgoing_synapse(self, handle: int) -> None:
        """Add an outgoing synapse handle to the neuron.
        
        Args:
            handle: The synapse handle
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.add_outgoing_synapse(200)
            >>> neuron.get_outgoing_synapses()
            [200]
        """
        self._outgoing_synapses.append(handle)
        
    def get_incoming_synapses(self) -> List[int]:
        """Get the vector of incoming synapse handles.
        
        Returns:
            list: List of incoming synapse handles
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.add_incoming_synapse(100)
            >>> neuron.get_incoming_synapses()
            [100]
        """
        return self._incoming_synapses.copy()
        
    def get_outgoing_synapses(self) -> List[int]:
        """Get the vector of outgoing synapse handles.
        
        Returns:
            list: List of outgoing synapse handles
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.add_outgoing_synapse(200)
            >>> neuron.get_outgoing_synapses()
            [200]
        """
        return self._outgoing_synapses.copy()
        
    def get_plasticity_flags(self) -> Dict[str, bool]:
        """Get the plasticity flags structure.
        
        Returns:
            dict: Dictionary containing plasticity flags
            
        Example:
            >>> neuron = Neuron()
            >>> flags = neuron.get_plasticity_flags()
            >>> flags['hebbian']
            False
        """
        return self._plasticity_flags.copy()
        
    def get_plasticity_flags_mutable(self) -> Dict[str, bool]:
        """Get mutable reference to plasticity flags (for modification).
        
        Returns:
            dict: Mutable reference to plasticity flags dictionary
            
        Example:
            >>> neuron = Neuron()
            >>> flags = neuron.get_plasticity_flags_mutable()
            >>> flags['hebbian'] = True
            >>> neuron.get_plasticity_flags()['hebbian']
            True
        """
        return self._plasticity_flags
        
    def enable_plasticity(self, hebbian: bool, stdp: bool, reward_modulated: bool) -> None:
        """Enable plasticity rules (Hebbian, STDP, reward-modulated).
        
        Args:
            hebbian: Whether to enable Hebbian plasticity
            stdp: Whether to enable STDP
            reward_modulated: Whether to enable reward-modulated plasticity
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.enable_plasticity(True, True, False)
        """
        self._plasticity_flags['hebbian'] = hebbian
        self._plasticity_flags['stdp'] = stdp
        self._plasticity_flags['reward_modulated'] = reward_modulated
        
    def set_region_id(self, region: RegionId) -> None:
        """Set the neuron's region ID.
        
        Args:
            region: The region ID
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_region_id(5)
        """
        self._region_id = region
        
    def get_region_id(self) -> RegionId:
        """Get the neuron's region ID.
        
        Returns:
            int: The region ID
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_region_id()
            0
        """
        return self._region_id
        
    def set_population_id(self, population: PopulationId) -> None:
        """Set the neuron's population ID.
        
        Args:
            population: The population ID
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.set_population_id(10)
        """
        self._population_id = population
        
    def get_population_id(self) -> PopulationId:
        """Get the neuron's population ID.
        
        Returns:
            int: The population ID
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.get_population_id()
            0
        """
        return self._population_id
        
    def step(self, current_time: Timestamp) -> None:
        """Update neuron for one simulation step.
        
        Args:
            current_time: The current simulation time (in seconds)
            
        Example:
            >>> neuron = Neuron()
            >>> neuron.step(0.1)
        """
        self.step_lif(current_time)
        self.clear_total_current()
        self.decrement_refractory()
        
    def reset(self) -> None:
        """Reset neuron to initial state.
        
        Example:
            >>> neuron = Neuron()
            >>> neuron.inject_current(1.0)
            >>> neuron.reset()
            >>> neuron.get_total_current()
            0.0
        """
        self._membrane_potential = -70.0
        self._firing_state = FiringState.Resting
        self._refractory_remaining = 0
        self._total_current = 0.0
        self._spike_history.clear()
        self._adaptation_variable = 0.0
        self._last_spike_time = -1.0
        
    def to_string(self) -> str:
        """Get a string representation of the neuron.
        
        Returns:
            str: String representation of the neuron
            
        Example:
            >>> neuron = Neuron(1)
            >>> str(neuron)
            'Neuron(id=1, type=Internal, potential=-70.0mV, firing=False, refractory=False)'
        """
        return (f"Neuron(id={self._id}, type={self._type}, "
                f"potential={self._membrane_potential:.1f}mV, "
                f"firing={self.is_firing()}, refractory={self.is_refractory()})")
        
    def __repr__(self) -> str:
        return self.to_string()
        
    def __str__(self) -> str:
        return self.to_string()
class Synapse:
    """Enhanced Synapse class with comprehensive Python bindings and API
    
    This class provides Python bindings for the C++ Synapse class with enhanced
    functionality including convenience methods, type safety, and additional
    features for easier Python usage.
    
    Features:
    - Complete mapping of all C++ Synapse methods
    - Pythonic convenience methods
    - Enhanced type checking and validation
    - Mutable plasticity flags for runtime modification
    - Enhanced string representations
    - Spike history management
    - Eligibility traces for learning
    - Convenience methods for common operations
    """
    
    _next_id = 1
    
    def __init__(self, synapse_id: Optional[SynapseId] = None, 
                 source_neuron: Optional[NeuronId] = None,
                 dest_neuron: Optional[NeuronId] = None):
        if synapse_id is None:
            synapse_id = Synapse._next_id
            Synapse._next_id += 1
            
        self._id: SynapseId = synapse_id
        self._source_neuron: NeuronId = source_neuron if source_neuron is not None else 0
        self._dest_neuron: NeuronId = dest_neuron if dest_neuron is not None else 0
        self._weight: SynapticWeight = 0.1
        self._delay: int = 1
        self._type: str = SynapseType.Excitatory
        self._pre_spike_history: List[Timestamp] = []
        self._post_spike_history: List[Timestamp] = []
        self._plasticity_flags: Dict[str, bool] = {
            'hebbian': False,
            'stdp': False,
            'reward_modulated': False,
            'structural': False,
            'eligible': False
        }
        self._eligibility_trace: float = 0.0
        self._efficacy: float = 1.0
        self._last_pre_spike: float = -1.0
        self._last_post_spike: float = -1.0
        
    def get_id(self) -> SynapseId:
        """Get the synapse's unique identifier.
        
        Returns:
            int: The synapse's ID
            
        Example:
            >>> synapse = Synapse(1, 2, 3)
            >>> synapse.get_id()
            1
        """
        return self._id
        
    def get_source_neuron(self) -> NeuronId:
        """Get the source neuron ID.
        
        Returns:
            int: The source neuron ID
            
        Example:
            >>> synapse = Synapse(1, 2, 3)
            >>> synapse.get_source_neuron()
            2
        """
        return self._source_neuron
        
    def get_destination_neuron(self) -> NeuronId:
        """Get the destination neuron ID.
        
        Returns:
            int: The destination neuron ID
            
        Example:
            >>> synapse = Synapse(1, 2, 3)
            >>> synapse.get_destination_neuron()
            3
        """
        return self._dest_neuron
        
    # Backward compatibility aliases
    def get_pre(self) -> NeuronId:
        """Get the source neuron ID (alias for get_source_neuron).
        
        Returns:
            int: The source neuron ID
            
        Example:
            >>> synapse = Synapse(1, 2, 3)
            >>> synapse.get_pre()
            2
        """
        return self._source_neuron
        
    def get_post(self) -> NeuronId:
        """Get the destination neuron ID (alias for get_destination_neuron).
        
        Returns:
            int: The destination neuron ID
            
        Example:
            >>> synapse = Synapse(1, 2, 3)
            >>> synapse.get_post()
            3
        """
        return self._dest_neuron
        
    def get_weight(self) -> SynapticWeight:
        """Get the synaptic weight (-1.0 to 1.0).
        
        Returns:
            float: The synaptic weight
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_weight()
            0.1
        """
        return self._weight
        
    def set_weight(self, weight: SynapticWeight) -> None:
        """Set the synaptic weight.
        
        Args:
            weight: The synaptic weight (-1.0 to 1.0)
            
        Raises:
            ValueError: If weight is outside valid range
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_weight(0.5)
            >>> synapse.get_weight()
            0.5
        """
        if weight < -1.0 or weight > 1.0:
            raise ValueError("Synaptic weight must be between -1.0 and 1.0")
        self._weight = weight
        
    def add_to_weight(self, delta: SynapticWeight) -> None:
        """Add to the synaptic weight (convenience method).
        
        Args:
            delta: The amount to add to weight
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_weight(0.5)
            >>> synapse.add_to_weight(0.2)
            >>> synapse.get_weight()
            0.7
        """
        self._weight = max(-1.0, min(1.0, self._weight + delta))
        
    def get_delay(self) -> int:
        """Get the synaptic delay in simulation steps.
        
        Returns:
            int: The synaptic delay in steps
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_delay()
            1
        """
        return self._delay
        
    def set_delay(self, delay: int) -> None:
        """Set the synaptic delay.
        
        Args:
            delay: The synaptic delay in steps
            
        Raises:
            ValueError: If delay is negative
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_delay(5)
            >>> synapse.get_delay()
            5
        """
        if delay < 0:
            raise ValueError("Delay must be non-negative")
        self._delay = delay
        
    def get_type(self) -> str:
        """Get the synapse type.
        
        Returns:
            str: The synapse type
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_type()
            'Excitatory'
        """
        return self._type
        
    def set_type(self, synapse_type: str) -> None:
        """Set the synapse type with validation.
        
        Args:
            synapse_type: The synapse type
            
        Raises:
            ValueError: If synapse_type is invalid
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_type('Inhibitory')
        """
        self._type = SynapseType.from_string(synapse_type)
        
    def is_excitatory(self) -> bool:
        """Check if synapse is excitatory.
        
        Returns:
            bool: True if synapse is excitatory, False otherwise
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.is_excitatory()
            True
        """
        return self._type == SynapseType.Excitatory
        
    def is_inhibitory(self) -> bool:
        """Check if synapse is inhibitory.
        
        Returns:
            bool: True if synapse is inhibitory, False otherwise
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_type('Inhibitory')
            >>> synapse.is_inhibitory()
            True
        """
        return self._type == SynapseType.Inhibitory
        
    def record_pre_spike(self, timestamp: Timestamp) -> None:
        """Record a presynaptic spike at the given timestamp.
        
        Args:
            timestamp: The spike timestamp (in seconds)
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.record_pre_spike(0.1)
            >>> synapse.get_pre_spike_history()
            [0.1]
        """
        self._pre_spike_history.append(timestamp)
        self._last_pre_spike = timestamp
        
    def record_post_spike(self, timestamp: Timestamp) -> None:
        """Record a postsynaptic spike at the given timestamp.
        
        Args:
            timestamp: The spike timestamp (in seconds)
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.record_post_spike(0.2)
            >>> synapse.get_post_spike_history()
            [0.2]
        """
        self._post_spike_history.append(timestamp)
        self._last_post_spike = timestamp
        
    def get_pre_spike_history(self) -> List[Timestamp]:
        """Get the vector of presynaptic spike timestamps.
        
        Returns:
            list: List of presynaptic spike timestamps
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.record_pre_spike(0.1)
            >>> synapse.record_pre_spike(0.2)
            >>> synapse.get_pre_spike_history()
            [0.1, 0.2]
        """
        return self._pre_spike_history.copy()
        
    def get_post_spike_history(self) -> List[Timestamp]:
        """Get the vector of postsynaptic spike timestamps.
        
        Returns:
            list: List of postsynaptic spike timestamps
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.record_post_spike(0.1)
            >>> synapse.record_post_spike(0.2)
            >>> synapse.get_post_spike_history()
            [0.1, 0.2]
        """
        return self._post_spike_history.copy()
        
    def clear_history(self) -> None:
        """Clear both spike history vectors.
        
        Example:
            >>> synapse = Synapse()
            >>> synapse.record_pre_spike(0.1)
            >>> synapse.clear_history()
            >>> synapse.get_pre_spike_history()
            []
        """
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        self._last_pre_spike = -1.0
        self._last_post_spike = -1.0
        
    def get_plasticity_flags(self) -> Dict[str, bool]:
        """Get the plasticity flags structure.
        
        Returns:
            dict: Dictionary containing plasticity flags
            
        Example:
            >>> synapse = Synapse()
            >>> flags = synapse.get_plasticity_flags()
            >>> flags['hebbian']
            False
        """
        return self._plasticity_flags.copy()
        
    def get_plasticity_flags_mutable(self) -> Dict[str, bool]:
        """Get mutable reference to plasticity flags (for modification).
        
        Returns:
            dict: Mutable reference to plasticity flags dictionary
            
        Example:
            >>> synapse = Synapse()
            >>> flags = synapse.get_plasticity_flags_mutable()
            >>> flags['hebbian'] = True
            >>> synapse.get_plasticity_flags()['hebbian']
            True
        """
        return self._plasticity_flags
        
    def enable_plasticity(self, hebbian: bool, stdp: bool, reward_modulated: bool) -> None:
        """Enable plasticity rules (Hebbian, STDP, reward-modulated).
        
        Args:
            hebbian: Whether to enable Hebbian plasticity
            stdp: Whether to enable STDP
            reward_modulated: Whether to enable reward-modulated plasticity
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.enable_plasticity(True, True, False)
        """
        self._plasticity_flags['hebbian'] = hebbian
        self._plasticity_flags['stdp'] = stdp
        self._plasticity_flags['reward_modulated'] = reward_modulated
        
    def get_eligibility_trace(self) -> float:
        """Get the eligibility trace value.
        
        Returns:
            float: The eligibility trace value
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_eligibility_trace()
            0.0
        """
        return self._eligibility_trace
        
    def set_eligibility_trace(self, trace: float) -> None:
        """Set the eligibility trace value.
        
        Args:
            trace: The eligibility trace value
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_eligibility_trace(0.5)
            >>> synapse.get_eligibility_trace()
            0.5
        """
        self._eligibility_trace = trace
        
    def decay_eligibility_trace(self, factor: float) -> None:
        """Decay the eligibility trace by the given factor.
        
        Args:
            factor: The decay factor (0 to 1)
            
        Raises:
            ValueError: If factor is not between 0 and 1
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_eligibility_trace(1.0)
            >>> synapse.decay_eligibility_trace(0.1)
            >>> synapse.get_eligibility_trace()
            0.9
        """
        if factor < 0 or factor > 1:
            raise ValueError("Decay factor must be between 0 and 1")
        self._eligibility_trace *= (1.0 - factor)
        if abs(self._eligibility_trace) < 0.001:
            self._eligibility_trace = 0.0
        
    def get_efficacy(self) -> float:
        """Get the synaptic efficacy.
        
        Returns:
            float: The synaptic efficacy
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_efficacy()
            1.0
        """
        return self._efficacy
        
    def set_efficacy(self, efficacy: float) -> None:
        """Set the synaptic efficacy.
        
        Args:
            efficacy: The synaptic efficacy (0 to 2)
            
        Raises:
            ValueError: If efficacy is outside valid range
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_efficacy(1.5)
            >>> synapse.get_efficacy()
            1.5
        """
        if efficacy < 0 or efficacy > 2:
            raise ValueError("Synaptic efficacy must be between 0.0 and 2.0")
        self._efficacy = efficacy
        
    def get_last_pre_spike(self) -> float:
        """Get the timestamp of the last presynaptic spike.
        
        Returns:
            float: The timestamp of the last presynaptic spike, or -1.0 if none
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_last_pre_spike()
            -1.0
        """
        return self._last_pre_spike
        
    def get_last_post_spike(self) -> float:
        """Get the timestamp of the last postsynaptic spike.
        
        Returns:
            float: The timestamp of the last postsynaptic spike, or -1.0 if none
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.get_last_post_spike()
            -1.0
        """
        return self._last_post_spike
        
    def step(self, current_time: Timestamp) -> None:
        """Update synapse for one simulation step.
        
        Args:
            current_time: The current simulation time (in seconds)
            
        Example:
            >>> synapse = Synapse()
            >>> synapse.step(0.1)
        """
        # Decay eligibility trace
        self._eligibility_trace *= 0.999
        if abs(self._eligibility_trace) < 0.001:
            self._eligibility_trace = 0.0
        
        # Decay efficacy slightly over time
        self._efficacy = max(0.8, self._efficacy * 0.999)
        
    def reset(self) -> None:
        """Reset synapse to initial state.
        
        Example:
            >>> synapse = Synapse()
            >>> synapse.set_weight(0.5)
            >>> synapse.set_eligibility_trace(1.0)
            >>> synapse.reset()
            >>> synapse.get_weight()
            0.1
        """
        self._weight = 0.1
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        self._eligibility_trace = 0.0
        self._efficacy = 1.0
        self._last_pre_spike = -1.0
        self._last_post_spike = -1.0
        
    def to_string(self) -> str:
        """Get a string representation of the synapse.
        
        Returns:
            str: String representation of the synapse
            
        Example:
            >>> synapse = Synapse(1, 2, 3)
            >>> str(synapse)
            'Synapse(id=1, source=2, dest=3, weight=0.100, type=Excitatory)'
        """
        return (f"Synapse(id={self._id}, source={self._source_neuron}, "
                f"dest={self._dest_neuron}, weight={self._weight:.3f}, "
                f"type={self._type})")
        
    def __repr__(self) -> str:
        return self.to_string()
        
    def __str__(self) -> str:
        return self.to_string()
# Utility functions for enhanced Python bindings
class EnhancedBindingsManager:
    """Manager for enhanced Python bindings with additional functionality"""
    
    def __init__(self):
        self._neurons = {}
        self._synapses = {}
        self._version = "1.0.0 Enhanced"
        
    def create_neuron(self, neuron_id: NeuronId) -> Neuron:
        """Create an enhanced neuron.
        
        Args:
            neuron_id: The neuron ID
            
        Returns:
            Neuron: Created neuron instance
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> neuron = manager.create_neuron(1)
        """
        neuron = Neuron(neuron_id)
        self._neurons[neuron_id] = neuron
        return neuron
        
    def create_synapse(self, synapse_id: SynapseId, source_neuron: NeuronId, dest_neuron: NeuronId) -> Synapse:
        """Create an enhanced synapse.
        
        Args:
            synapse_id: The synapse ID
            source_neuron: The source neuron ID
            dest_neuron: The destination neuron ID
            
        Returns:
            Synapse: Created synapse instance
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> synapse = manager.create_synapse(1, 2, 3)
        """
        synapse = Synapse(synapse_id, source_neuron, dest_neuron)
        self._synapses[synapse_id] = synapse
        return synapse
        
    def get_neuron(self, neuron_id: NeuronId) -> Optional[Neuron]:
        """Get a neuron by ID.
        
        Args:
            neuron_id: The neuron ID
            
        Returns:
            Neuron or None: The neuron if found, None otherwise
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> neuron = manager.create_neuron(1)
            >>> retrieved = manager.get_neuron(1)
        """
        return self._neurons.get(neuron_id)
        
    def get_synapse(self, synapse_id: SynapseId) -> Optional[Synapse]:
        """Get a synapse by ID.
        
        Args:
            synapse_id: The synapse ID
            
        Returns:
            Synapse or None: The synapse if found, None otherwise
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> synapse = manager.create_synapse(1, 2, 3)
            >>> retrieved = manager.get_synapse(1)
        """
        return self._synapses.get(synapse_id)
        
    def get_all_neurons(self) -> List[Neuron]:
        """Get all neurons.
        
        Returns:
            list: List of all neurons
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> manager.create_neuron(1)
            >>> manager.create_neuron(2)
            >>> all_neurons = manager.get_all_neurons()
        """
        return list(self._neurons.values())
        
    def get_all_synapses(self) -> List[Synapse]:
        """Get all synapses.
        
        Returns:
            list: List of all synapses
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> manager.create_synapse(1, 2, 3)
            >>> manager.create_synapse(2, 3, 4)
            >>> all_synapses = manager.get_all_synapses()
        """
        return list(self._synapses.values())
        
    def get_version(self) -> str:
        """Get the enhanced bindings version.
        
        Returns:
            str: Version string
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> manager.get_version()
            '1.0.0 Enhanced'
        """
        return self._version
        
    def get_bindings_info(self) -> Dict[str, Any]:
        """Get information about enhanced bindings.
        
        Returns:
            dict: Dictionary containing binding information and features
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> info = manager.get_bindings_info()
            >>> info['version']
            '1.0.0 Enhanced'
        """
        return {
            "version": self._version,
            "neuron_class": Neuron.__doc__,
            "synapse_class": Synapse.__doc__,
            "features": [
                "Enhanced neuron dynamics",
                "Advanced plasticity controls",
                "Synaptic eligibility traces",
                "Convenience methods",
                "Mutable plasticity flags",
                "Enhanced string representations"
            ],
            "neuron_methods": [method for method in dir(Neuron) 
                             if not method.startswith('_')],
            "synapse_methods": [method for method in dir(Synapse) 
                             if not method.startswith('_')]
        }
        
    def simulate_step(self, dt: float = 0.001) -> Dict[str, Any]:
        """Simulate one timestep across all neurons.
        
        Args:
            dt: Time step duration (in seconds)
            
        Returns:
            dict: Simulation statistics
            
        Example:
            >>> manager = EnhancedBindingsManager()
            >>> manager.create_neuron(1)
            >>> stats = manager.simulate_step(0.001)
        """
        current_time = time.time()
        stats = {
            "neurons_updated": 0,
            "spikes_generated": 0,
            "current_time": current_time
        }
        
        for neuron in self._neurons.values():
            if neuron.step(current_time):
                stats["spikes_generated"] += 1
                # Update connected synapses
                for synapse_id in neuron.get_outgoing_synapses():
                    synapse = self._synapses.get(synapse_id)
                    if synapse:
                        synapse.record_pre_spike(current_time)
            stats["neurons_updated"] += 1
            
        return stats
# Global bindings manager
_bindings_manager = EnhancedBindingsManager()

# Convenience functions for enhanced Python bindings
def create_enhanced_neuron(neuron_id: NeuronId) -> Neuron:
    """Convenience function to create an enhanced neuron.
    
    Args:
        neuron_id: The neuron ID
        
    Returns:
        Neuron: Created neuron instance
        
    Example:
        >>> neuron = create_enhanced_neuron(1)
    """
    return _bindings_manager.create_neuron(neuron_id)

def create_enhanced_synapse(synapse_id: SynapseId, source_neuron: NeuronId, dest_neuron: NeuronId) -> Synapse:
    """Convenience function to create an enhanced synapse.
    
    Args:
        synapse_id: The synapse ID
        source_neuron: The source neuron ID
        dest_neuron: The destination neuron ID
        
    Returns:
        Synapse: Created synapse instance
        
    Example:
        >>> synapse = create_enhanced_synapse(1, 2, 3)
    """
    return _bindings_manager.create_synapse(synapse_id, source_neuron, dest_neuron)

def get_enhanced_bindings_manager() -> EnhancedBindingsManager:
    """Get the global bindings manager.
    
    Returns:
        EnhancedBindingsManager: Global bindings manager instance
        
    Example:
        >>> manager = get_enhanced_bindings_manager()
    """
    return _bindings_manager

def get_version() -> str:
    """Get the enhanced Python bindings version.
    
    Returns:
        str: Version string
        
    Example:
        >>> get_version()
        '1.0.0 Enhanced'
    """
    return _bindings_manager.get_version()

def get_bindings_info() -> Dict[str, Any]:
    """Get information about enhanced Python bindings.
    
    Returns:
        dict: Dictionary containing binding information and features
        
    Example:
        >>> info = get_bindings_info()
        >>> info['version']
        '1.0.0 Enhanced'
    """
    return _bindings_manager.get_bindings_info()

# Example usage and demonstration
if __name__ == "__main__":
    print("Enhanced Python Bindings for Neuron and Synapse Classes")
    print("=" * 60)
    
    # Create enhanced bindings manager
    manager = get_enhanced_bindings_manager()
    
    # Get bindings information
    info = get_bindings_info()
    print(f"Version: {info['version']}")
    print(f"Features: {', '.join(info['features'])}")
    
    # Create example neurons
    neuron1 = create_enhanced_neuron(1)
    neuron1.set_type(NeuronType.Excitatory)
    neuron1.set_threshold(-50.0)
    
    neuron2 = create_enhanced_neuron(2)
    neuron2.set_type(NeuronType.Inhibitory)
    neuron2.set_threshold(-55.0)
    
    # Create example synapses
    synapse1 = create_enhanced_synapse(1, 1, 2)
    synapse1.set_weight(0.5)
    
    synapse2 = create_enhanced_synapse(2, 2, 1)
    synapse2.set_weight(-0.3)
    
    print(f"\nCreated neurons: {neuron1.get_id()}, {neuron2.get_id()}")
    print(f"Created synapses: {synapse1.get_id()}, {synapse2.get_id()}")
    
    # Simulate some activity
    for i in range(100):
        neuron1.inject_current(0.5)
        neuron2.inject_current(-0.2)
        
        if neuron1.check_threshold():
            neuron1.record_spike(time.time())
            synapse1.record_post_spike(time.time())
            
        if neuron2.check_threshold():
            neuron2.record_spike(time.time())
            synapse2.record_post_spike(time.time())
            
        neuron1.step(time.time())
        neuron2.step(time.time())
        synapse1.step(time.time())
        synapse2.step(time.time())
    
    print(f"\nSimulation results:")
    print(f"Neuron 1 spikes: {len(neuron1.get_spike_history())}")
    print(f"Neuron 2 spikes: {len(neuron2.get_spike_history())}")
    print(f"Synapse 1 pre-spikes: {len(synapse1.get_pre_spike_history())}")
    print(f"Synapse 1 post-spikes: {len(synapse1.get_post_spike_history())}")
    
    print(f"\nNeuron 1: {neuron1}")
    print(f"Neuron 2: {neuron2}")
    print(f"Synapse 1: {synapse1}")
    print(f"Synapse 2: {synapse2}")
    
    print("\nEnhanced bindings demonstration complete!")
"""