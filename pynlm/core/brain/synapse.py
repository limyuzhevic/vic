"""NLM Synapse class - Synaptic connections between neurons."""

import math
from typing import List, Optional
from enum import Enum

from .enums import SynapseType
from .exceptions import NLMException


class Synapse:
    """Synaptic connection with plasticity and dynamics."""
    
    def __init__(self, synapse_id: int, source_neuron: int, dest_neuron: int):
        """
        Initialize synapse.
        
        Args:
            synapse_id: Unique identifier
            source_neuron: Presynaptic neuron ID
            dest_neuron: Postsynaptic neuron ID
        """
        self._synapse_id = synapse_id
        self._source_neuron = source_neuron
        self._dest_neuron = dest_neuron
        
        # Synaptic properties
        self._weight = 0.0
        self._delay = 1  # Default 1 timestep delay
        self._synapse_type = SynapseType.EXCITATORY
        
        # Dynamics state
        self._efficacy = 1.0
        self._short_term_depression = 1.0
        self._short_term_facilitation = 0.0
        self._last_pre_spike_time = -1.0
        self._last_post_spike_time = -1.0
        self._eligibility_trace = 0.0
        
        # Plasticity flags
        self._hebbian_enabled = False
        self._stdp_enabled = False
        self._reward_modulated = False
        
        # Spike history for STDP
        self._pre_spike_history = []
        self._post_spike_history = []
        
        # Constants
        self._MIN_WEIGHT = -1.0
        self._MAX_WEIGHT = 1.0
        self._MIN_EFFICIENCY = 0.0
        self._MAX_EFFICIENCY = 2.0
        
        # STP parameters
        self._STP_FACILITATION_TAU = 100.0  # ms
        self._STP_DEPRESSION_TAU = 200.0    # ms
        self._STP_U_MAX = 1.0
        
        # For simulation time tracking
        self._current_time = 0.0
        
    def step(self, current_time: float):
        """
        Advance synapse dynamics.
        
        Args:
            current_time: Current simulation time (ms)
        """
        self._current_time = current_time
        
        # Update short-term plasticity
        self._update_short_term_plasticity(current_time)
        
        # Decay eligibility trace for reward-modulated learning
        self._eligibility_trace *= (1.0 - 0.001)  # Fast decay
        if abs(self._eligibility_trace) < 0.001:
            self._eligibility_trace = 0.0
        
        # Clamp weight
        self._weight = max(self._MIN_WEIGHT, min(self._MAX_WEIGHT, self._weight))
        
    def _update_short_term_plasticity(self, current_time: float):
        """Update short-term plasticity dynamics."""
        if self._last_pre_spike_time >= 0:
            time_since_pre = current_time - self._last_pre_spike_time
            self._short_term_facilitation *= math.exp(-time_since_pre / self._STP_FACILITATION_TAU)
        
        if self._last_pre_spike_time >= 0 or self._last_post_spike_time >= 0:
            time_since_activity = max(
                self._last_post_spike_time if self._last_post_spike_time >= 0 else 0,
                self._last_pre_spike_time if self._last_pre_spike_time >= 0 else 0
            )
            if self._last_post_spike_time >= 0:
                time_since_activity = current_time - self._last_post_spike_time
            elif self._last_pre_spike_time >= 0:
                time_since_activity = current_time - self._last_pre_spike_time
            
            # Recovery from depression toward 1.0
            self._short_term_depression += (1.0 - self._short_term_depression) * \
                (1.0 - math.exp(-time_since_activity / self._STP_DEPRESSION_TAU))
        
    def record_pre_spike(self, timestamp: float):
        """
        Record presynaptic spike.
        
        Args:
            timestamp: Spike timestamp (ms)
        """
        self._pre_spike_history.append(timestamp)
        self._last_pre_spike_time = timestamp
        self._update_short_term_plasticity(timestamp)
        
    def record_post_spike(self, timestamp: float):
        """
        Record postsynaptic spike.
        
        Args:
            timestamp: Spike timestamp (ms)
        """
        self._post_spike_history.append(timestamp)
        self._last_post_spike_time = timestamp
        self._update_short_term_plasticity(timestamp)
        
    def get_weight(self) -> float:
        """Get current synaptic weight."""
        return self._weight
        
    def set_weight(self, weight: float):
        """Set synaptic weight."""
        self._weight = weight
        
    def add_to_weight(self, delta: float):
        """Add delta to current weight."""
        self._weight += delta
        self._weight = max(self._MIN_WEIGHT, min(self._MAX_WEIGHT, self._weight))
        
    def get_delay(self) -> int:
        """Get synaptic delay (in timesteps)."""
        return self._delay
        
    def set_delay(self, delay: int):
        """Set synaptic delay."""
        self._delay = max(0, delay)
        
    def get_type(self) -> SynapseType:
        """Get synapse type."""
        return self._synapse_type
        
    def set_type(self, synapse_type: SynapseType):
        """Set synapse type."""
        self._synapse_type = synapse_type
        
    def is_excitatory(self) -> bool:
        """Check if synapse is excitatory."""
        return self._synapse_type == SynapseType.EXCITATORY
        
    def is_inhibitory(self) -> bool:
        """Check if synapse is inhibitory."""
        return self._synapse_type == SynapseType.INHIBITORY
        
    def get_efficacy(self) -> float:
        """Get synaptic efficacy."""
        return self._efficacy
        
    def set_efficacy(self, efficacy: float):
        """Set synaptic efficacy."""
        self._efficacy = max(self._MIN_EFFICIENCY, min(self._MAX_EFFICIENCY, efficacy))
        
    def get_eligibility_trace(self) -> float:
        """Get eligibility trace."""
        return self._eligibility_trace
        
    def set_eligibility_trace(self, trace: float):
        """Set eligibility trace."""
        self._eligibility_trace = trace
        
    def decay_eligibility_trace(self, decay_rate: float):
        """Decay eligibility trace."""
        self._eligibility_trace *= (1.0 - decay_rate)
        if abs(self._eligibility_trace) < 0.001:
            self._eligibility_trace = 0.0
            
    def get_pre_spike_history(self) -> List[float]:
        """Get presynaptic spike history."""
        return self._pre_spike_history.copy()
        
    def get_post_spike_history(self) -> List[float]:
        """Get postsynaptic spike history."""
        return self._post_spike_history.copy()
        
    def clear_history(self):
        """Clear spike history."""
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        
    def reset(self):
        """Reset synapse to initial state."""
        self._weight = 0.0
        self._efficacy = 1.0
        self._short_term_depression = 1.0
        self._short_term_facilitation = 0.0
        self._last_pre_spike_time = -1.0
        self._last_post_spike_time = -1.0
        self._eligibility_trace = 0.0
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        
    def get_id(self) -> int:
        """Get synapse ID."""
        return self._synapse_id
        
    def get_source_neuron(self) -> int:
        """Get source neuron ID."""
        return self._source_neuron
        
    def get_destination_neuron(self) -> int:
        """Get destination neuron ID."""
        return self._dest_neuron
        
    def enable_plasticity(self, hebbian: bool = False, 
                          stdp: bool = False, 
                          reward_modulated: bool = False):
        """
        Enable plasticity mechanisms.
        
        Args:
            hebbian: Enable Hebbian learning
            stdp: Enable STDP
            reward_modulated: Enable reward-modulated plasticity
        """
        self._hebbian_enabled = hebbian
        self._stdp_enabled = stdp
        self._reward_modulated = reward_modulated


# Factory function
def create_synapse(synapse_id: int, source_neuron: int, 
                  dest_neuron: int) -> Synapse:
    """Create a new synapse."""
    return Synapse(synapse_id, source_neuron, dest_neuron)