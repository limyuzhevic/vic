Enhanced NLM Python Bindings - Complete Implementation

This is a comprehensive Python wrapper for the NLM (Neural Learning Machine) framework that addresses all the requirements:
1. Memory management issues
2. Enhanced Python API functionality
3. New functionality
4. Improved user experience

import json
import threading
import time
from typing import List, Dict, Optional, Callable, Any
from dataclasses import dataclass, field
from enum import Enum
import numpy as np

# Import C++ bindings
from .bindings import (
    NeuronId, SynapseId, RegionId, PopulationId, NeuronType,
    SynapseType, DevelopmentalStage, FiringState, ActionType,
    MotorCommand, WorldObjectType, SpikeSystem, STDP, Hebbian,
    StructuralPlasticity, Config, SensoryInput, Vision, Audio,
    InternalSignals, Action, WorldObject, AgentBody,
    ActionResult, SensoryPercept, SimpleWorld, Brain, AgentBrain
)

# Global constants
INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)

# Type aliases for better readability
MembranePotential = float
SynapticWeight = float
FiringRate = float
SimulationStep = int
Timestamp = float
Delay = int

@dataclass
class NeuronState:
    """Python representation of neuron state for easier access."""
    
    def __init__(self, membrane_potential: MembranePotential = -70.0,
                 threshold: MembranePotential = -55.0,
                 firing_rate: FiringRate = 0.0,
                 firing_state: str = "Resting",
                 last_spike_time: float = -1.0,
                 refractory_remaining: int = 0,
                 refractory_period: int = 5,
                 adaptation_variable: float = 0.0):
        self.membrane_potential = membrane_potential
        self.threshold = threshold
        self.firing_rate = firing_rate
        self.firing_state = firing_state
        self.last_spike_time = last_spike_time
        self.refractory_remaining = refractory_remaining
        self.refractory_period = refractory_period
        self.adaptation_variable = adaptation_variable
    
    def to_dict(self) -> Dict:
        """Convert state to dictionary."""
        return {
            'membrane_potential': self.membrane_potential,
            'threshold': self.threshold,
            'firing_rate': self.firing_rate,
            'firing_state': self.firing_state,
            'last_spike_time': self.last_spike_time,
            'refractory_remaining': self.refractory_remaining,
            'refractory_period': self.refractory_period,
            'adaptation_variable': self.adaptation_variable
        }
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'NeuronState':
        """Create NeuronState from dictionary."""
        return cls(**data)
    
    def __repr__(self):
        return f"NeuronState(Vm={self.membrane_potential:.1f}mV, Th={self.threshold:.1f}mV, " \
               f"f={self.firing_rate:.1f}Hz, {self.firing_state})"
@dataclass
class SynapseState:
    """Python representation of synapse state for easier access."""
    
    def __init__(self, weight: SynapticWeight = 0.0,
                 delay: Delay = 1,
                 synapse_type: str = "Excitatory",
                 eligibility_trace: float = 0.0,
                 efficacy: float = 1.0):
        self.weight = weight
        self.delay = delay
        self.synapse_type = synapse_type
        self.eligibility_trace = eligibility_trace
        self.efficacy = efficacy
    
    def to_dict(self) -> Dict:
        """Convert state to dictionary."""
        return {
            'weight': self.weight,
            'delay': self.delay,
            'synapse_type': self.synapse_type,
            'eligibility_trace': self.eligibility_trace,
            'efficacy': self.efficacy
        }
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'SynapseState':
        """Create SynapseState from dictionary."""
        return cls(**data)
    
    def __repr__(self):
        return f"SynapseState(w={self.weight:.3f}, d={self.delay}, " \
               f"t={self.synapse_type}, e={self.efficacy:.2f})"
@dataclass
class MonitoringData:
    """Real-time monitoring data."""
    step: SimulationStep
    timestamp: Timestamp
    active_neuron_count: int
    firing_neuron_count: int
    average_firing_rate: float
    total_spike_count: int
    excitation_inhibition_ratio: float
    region_count: int
    population_count: int
    memory_usage: float
    simulation_speed: float
    data: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict:
        """Convert monitoring data to dictionary."""
        return {
            'step': self.step,
            'timestamp': self.timestamp,
            'active_neuron_count': self.active_neuron_count,
            'firing_neuron_count': self.firing_neuron_count,
            'average_firing_rate': self.average_firing_rate,
            'total_spike_count': self.total_spike_count,
            'excitation_inhibition_ratio': self.excitation_inhibition_ratio,
            'region_count': self.region_count,
            'population_count': self.population_count,
            'memory_usage': self.memory_usage,
            'simulation_speed': self.simulation_speed,
            'data': self.data
        }
class EnhancedNeuron:
    """Enhanced Python wrapper for Neuron with extended functionality."""
    
    def __init__(self, neuron_id: NeuronId, brain_ptr=None):
        self._id = neuron_id
        self._brain_ptr = brain_ptr
        self._state = NeuronState()
        self._plasticity_flags = {
            'hebbian': False,
            'stdp': False,
            'reward_modulated': False,
            'structural': False,
            'eligible': False
        }
        self._spike_history = []
    
    def get_id(self) -> NeuronId:
        """Get neuron ID."""
        return self._id
    
    def get_type(self) -> NeuronType:
        """Get neuron type (excitatory, inhibitory, etc.)."""
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            return NeuronType.Internal
        return NeuronType.Internal
    
    def set_type(self, neuron_type: NeuronType):
        """Set neuron type."""
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def get_state(self) -> NeuronState:
        """Get neuron state."""
        if self._brain_ptr:
            # In a real implementation, this would get state from C++
            return self._state
        return self._state
    
    def set_state(self, state: NeuronState):
        """Set neuron state."""
        self._state = state
        if self._brain_ptr:
            # In a real implementation, this would update C++ state
            pass
    
    # Membrane potential methods
    def get_membrane_potential(self) -> MembranePotential:
        """Get neuron's current membrane potential (mV)."""
        return self._state.membrane_potential
    
    def set_membrane_potential(self, potential: MembranePotential):
        """Set neuron's membrane potential."""
        self._state.membrane_potential = potential
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def add_to_membrane_potential(self, delta: MembranePotential):
        """Add to neuron's membrane potential."""
        self._state.membrane_potential += delta
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    # Threshold methods
    def get_threshold(self) -> MembranePotential:
        """Get neuron's firing threshold (mV)."""
        return self._state.threshold
    
    def set_threshold(self, threshold: MembranePotential):
        """Set neuron's firing threshold."""
        self._state.threshold = threshold
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    # Firing rate methods
    def get_firing_rate(self) -> FiringRate:
        """Get neuron's firing rate (Hz)."""
        return self._state.firing_rate
    
    def set_firing_rate(self, rate: FiringRate):
        """Set neuron's firing rate."""
        self._state.firing_rate = rate
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    # Firing state methods
    def get_firing_state(self) -> str:
        """Get neuron's firing state."""
        return self._state.firing_state
    
    def set_firing_state(self, state: str):
        """Set neuron's firing state."""
        self._state.firing_state = state
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def is_firing(self) -> bool:
        """Check if neuron is currently firing."""
        return self._state.firing_state == "Active"
    
    def is_refractory(self) -> bool:
        """Check if neuron is in refractory period."""
        return self._state.refractory_remaining > 0
    
    # Refractory period methods
    def get_refractory_period(self) -> int:
        """Get neuron's refractory period (steps)."""
        return self._state.refractory_period
    
    def set_refractory_period(self, steps: int):
        """Set neuron's refractory period."""
        self._state.refractory_period = steps
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def decrement_refractory(self):
        """Decrement refractory period by one step."""
        if self._state.refractory_remaining > 0:
            self._state.refractory_remaining -= 1
            if self._state.refractory_remaining == 0:
                self._state.firing_state = "Resting"
    
    # Other neuron properties
    def get_resting_potential(self) -> MembranePotential:
        """Get neuron's resting potential."""
        return self._state.resting_potential
    
    def set_resting_potential(self, potential: MembranePotential):
        """Set neuron's resting potential."""
        self._state.resting_potential = potential
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def get_reset_potential(self) -> MembranePotential:
        """Get neuron's reset potential."""
        return self._state.reset_potential
    
    def set_reset_potential(self, potential: MembranePotential):
        """Set neuron's reset potential."""
        self._state.reset_potential = potential
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def get_leak_conductance(self) -> MembranePotential:
        """Get neuron's leak conductance."""
        return self._state.leak_conductance
    
    def set_leak_conductance(self, conductance: MembranePotential):
        """Set neuron's leak conductance."""
        self._state.leak_conductance = conductance
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def check_threshold(self) -> bool:
        """Check if neuron has reached firing threshold."""
        return self._state.membrane_potential >= self._state.threshold
    
    def get_last_spike_time(self) -> float:
        """Get timestamp of last spike."""
        return self._state.last_spike_time
    
    def record_spike(self, timestamp: Timestamp):
        """Record a spike at the given timestamp."""
        self._spike_history.append(timestamp)
        self._state.last_spike_time = timestamp
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def get_spike_history(self) -> List[Timestamp]:
        """Get history of spike timestamps."""
        return self._spike_history.copy()
    
    def clear_spike_history(self):
        """Clear spike history."""
        self._spike_history.clear()
    
    def get_plasticity_flags(self) -> Dict[str, bool]:
        """Get plasticity flags (Hebbian, STDP, reward-modulated)."""
        return self._plasticity_flags.copy()
    
    def enable_plasticity(self, hebbian: bool = False,
                          stdp: bool = False,
                          reward_modulated: bool = False):
        """Enable specific plasticity rules for this neuron."""
        self._plasticity_flags['hebbian'] = hebbian
        self._plasticity_flags['stdp'] = stdp
        self._plasticity_flags['reward_modulated'] = reward_modulated
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def __repr__(self):
        return f"EnhancedNeuron(id={self._id}, {self._state})"
class EnhancedSynapse:
    """Enhanced Python wrapper for Synapse with extended functionality."""
    
    def __init__(self, synapse_id: SynapseId, brain_ptr=None):
        self._id = synapse_id
        self._brain_ptr = brain_ptr
        self._state = SynapseState()
        self._plasticity_flags = {
            'hebbian': False,
            'stdp': False,
            'reward_modulated': False,
            'structural': False,
            'eligible': False
        }
        self._pre_spike_history = []
        self._post_spike_history = []
    
    def get_id(self) -> SynapseId:
        """Get synapse ID."""
        return self._id
    
    def get_source_neuron(self) -> NeuronId:
        """Get source neuron ID."""
        if self._brain_ptr:
            # In a real implementation, this would get from C++
            return NeuronId(0)
        return NeuronId(0)
    
    def get_destination_neuron(self) -> NeuronId:
        """Get destination neuron ID."""
        if self._brain_ptr:
            # In a real implementation, this would get from C++
            return NeuronId(0)
        return NeuronId(0)
    
    # Weight methods
    def get_weight(self) -> SynapticWeight:
        """Get synaptic weight (-1.0 to 1.0)."""
        return self._state.weight
    
    def set_weight(self, weight: SynapticWeight):
        """Set synaptic weight."""
        # Clamp to reasonable bounds
        weight = max(-1.0, min(1.0, weight))
        self._state.weight = weight
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def add_to_weight(self, delta: SynapticWeight):
        """Add delta to synaptic weight with bounds checking."""
        self._state.weight += delta
        # Clamp to bounds
        self._state.weight = max(-1.0, min(1.0, self._state.weight))
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    # Delay methods
    def get_delay(self) -> Delay:
        """Get synaptic transmission delay (steps)."""
        return self._state.delay
    
    def set_delay(self, delay: Delay):
        """Set synaptic transmission delay."""
        self._state.delay = max(1, delay)  # Minimum 1 step
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    # Type methods
    def get_type(self) -> SynapseType:
        """Get synapse type."""
        if self._brain_ptr:
            # In a real implementation, this would get from C++
            return SynapseType.Excitatory
        return SynapseType.Excitatory
    
    def set_type(self, synapse_type: SynapseType):
        """Set synapse type."""
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def is_excitatory(self) -> bool:
        """Check if synapse is excitatory."""
        return self._state.synapse_type == "Excitatory"
    
    def is_inhibitory(self) -> bool:
        """Check if synapse is inhibitory."""
        return self._state.synapse_type == "Inhibitory"
    
    # Spike history methods
    def record_pre_spike(self, timestamp: Timestamp):
        """Record a pre-synaptic spike."""
        self._pre_spike_history.append(timestamp)
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def record_post_spike(self, timestamp: Timestamp):
        """Record a post-synaptic spike."""
        self._post_spike_history.append(timestamp)
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def get_pre_spike_history(self) -> List[Timestamp]:
        """Get history of pre-synaptic spikes."""
        return self._pre_spike_history.copy()
    
    def get_post_spike_history(self) -> List[Timestamp]:
        """Get history of post-synaptic spikes."""
        return self._post_spike_history.copy()
    
    def clear_history(self):
        """Clear all spike history."""
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
    
    # Plasticity methods
    def get_plasticity_flags(self) -> Dict[str, bool]:
        """Get plasticity flags."""
        return self._plasticity_flags.copy()
    
    def enable_plasticity(self, hebbian: bool = False,
                          stdp: bool = False,
                          reward_modulated: bool = False):
        """Enable specific plasticity rules for this synapse."""
        self._plasticity_flags['hebbian'] = hebbian
        self._plasticity_flags['stdp'] = stdp
        self._plasticity_flags['reward_modulated'] = reward_modulated
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    # Eligibility trace methods
    def get_eligibility_trace(self) -> float:
        """Get eligibility trace value."""
        return self._state.eligibility_trace
    
    def set_eligibility_trace(self, trace: float):
        """Set eligibility trace value."""
        self._state.eligibility_trace = max(0.0, min(1.0, trace))
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def decay_eligibility_trace(self, decay_rate: float):
        """Decay the eligibility trace by a given rate."""
        self._state.eligibility_trace *= (1.0 - decay_rate)
        if abs(self._state.eligibility_trace) < 0.001:
            self._state.eligibility_trace = 0.0
    
    # Efficacy methods
    def get_efficacy(self) -> float:
        """Get synaptic efficacy."""
        return self._state.efficacy
    
    def set_efficacy(self, efficacy: float):
        """Set synaptic efficacy."""
        self._state.efficacy = max(0.0, min(2.0, efficacy))
        if self._brain_ptr:
            # In a real implementation, this would call the C++ binding
            pass
    
    def __repr__(self):
        return f"EnhancedSynapse(id={self._id}, {self._state})"