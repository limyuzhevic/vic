"""
Synapse implementation for NLM.
"""

from typing import List, Optional
from dataclasses import dataclass
from .types import *
from ..random import RandomGenerator
import random

class Synapse:
    """
    Synaptic connection between neurons.
    
    Represents a biological synapse with transmission delays,
    plasticity mechanisms, and short-term dynamics.
    """
    
    def __init__(self, synapse_id: int, source_neuron: int, dest_neuron: int):
        """
        Initialize synapse with unique IDs.
        
        Args:
            synapse_id: Unique identifier for the synapse
            source_neuron: Source neuron ID
            dest_neuron: Destination neuron ID
        """
        self._synapse_id = synapse_id
        self._source_neuron = source_neuron
        self._dest_neuron = dest_neuron
        self._weight = 1.0
        self._delay = 1
        self._type = SynapseType.EXCITATORY
        self._plasticity_flags = PlasticityFlags()
        self._eligibility_trace = 0.0
        self._efficacy = 1.0
        self._pre_spike_history: List[float] = []
        self._post_spike_history: List[float] = []
        self._last_transmission_time = -1.0
        self._transmission_queue: List[float] = []
    
    def get_id(self) -> int:
        """
        Get synapse identifier.
        
        Returns:
            Synapse ID
        """
        return self._synapse_id
    
    def get_source_neuron(self) -> int:
        """
        Get source neuron identifier.
        
        Returns:
            Source neuron ID
        """
        return self._source_neuron
    
    def get_destination_neuron(self) -> int:
        """
        Get destination neuron identifier.
        
        Returns:
            Destination neuron ID
        """
        return self._dest_neuron
    
    def get_weight(self) -> float:
        """
        Get synaptic weight.
        
        Returns:
            Synaptic weight
        """
        return self._weight
    
    def set_weight(self, weight: float) -> None:
        """
        Set synaptic weight.
        
        Args:
            weight: Synaptic weight
        """
        self._weight = max(0.0, weight)  # Weights should be non-negative
    
    def add_to_weight(self, delta: float) -> None:
        """
        Add to synaptic weight.
        
        Args:
            delta: Weight change
        """
        self._weight = max(0.0, self._weight + delta)
    
    def get_delay(self) -> int:
        """
        Get transmission delay.
        
        Returns:
            Delay in simulation steps
        """
        return self._delay
    
    def set_delay(self, delay: int) -> None:
        """
        Set transmission delay.
        
        Args:
            delay: Delay in simulation steps
        """
        self._delay = max(0, delay)
    
    def get_type(self) -> SynapseType:
        """
        Get synapse type.
        
        Returns:
            Synapse type
        """
        return self._type
    
    def set_type(self, synapse_type: SynapseType) -> None:
        """
        Set synapse type.
        
        Args:
            synapse_type: Synapse type
        """
        self._type = synapse_type
    
    def is_excitatory(self) -> bool:
        """
        Check if synapse is excitatory.
        
        Returns:
            True if excitatory, False otherwise
        """
        return self._type == SynapseType.EXCITATORY
    
    def is_inhibitory(self) -> bool:
        """
        Check if synapse is inhibitory.
        
        Returns:
            True if inhibitory, False otherwise
        """
        return self._type == SynapseType.INHIBITORY
    
    def record_pre_spike(self, timestamp: float) -> None:
        """
        Record presynaptic spike.
        
        Args:
            timestamp: Spike time
        """
        self._pre_spike_history.append(timestamp)
        if len(self._pre_spike_history) > 100:
            self._pre_spike_history.pop(0)
    
    def record_post_spike(self, timestamp: float) -> None:
        """
        Record postsynaptic spike.
        
        Args:
            timestamp: Spike time
        """
        self._post_spike_history.append(timestamp)
        if len(self._post_spike_history) > 100:
            self._post_spike_history.pop(0)
    
    def get_pre_spike_history(self) -> List[float]:
        """
        Get presynaptic spike history.
        
        Returns:
            List of presynaptic spike times
        """
        return self._pre_spike_history.copy()
    
    def get_post_spike_history(self) -> List[float]:
        """
        Get postsynaptic spike history.
        
        Returns:
            List of postsynaptic spike times
        """
        return self._post_spike_history.copy()
    
    def clear_history(self) -> None:
        """Clear spike history."""
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
    
    def get_plasticity_flags(self) -> PlasticityFlags:
        """
        Get plasticity flags.
        
        Returns:
            Plasticity flags
        """
        return self._plasticity_flags
    
    def enable_plasticity(self, hebbian: bool, stdp: bool, 
                         reward_modulated: bool) -> None:
        """
        Enable plasticity mechanisms.
        
        Args:
            hebbian: Enable Hebbian plasticity
            stdp: Enable STDP
            reward_modulated: Enable reward-modulated plasticity
        """
        self._plasticity_flags.hebbian = hebbian
        self._plasticity_flags.stdp = stdp
        self._plasticity_flags.reward_modulated = reward_modulated
    
    def get_eligibility_trace(self) -> float:
        """
        Get eligibility trace.
        
        Returns:
            Eligibility trace value
        """
        return self._eligibility_trace
    
    def set_eligibility_trace(self, trace: float) -> None:
        """
        Set eligibility trace.
        
        Args:
            trace: Eligibility trace value
        """
        self._eligibility_trace = trace
    
    def decay_eligibility_trace(self, decay_rate: float) -> None:
        """
        Decay eligibility trace.
        
        Args:
            decay_rate: Decay rate (0.0 to 1.0)
        """
        self._eligibility_trace *= (1.0 - decay_rate)
    
    def get_efficacy(self) -> float:
        """
        Get synaptic efficacy.
        
        Returns:
            Synaptic efficacy
        """
        return self._efficacy
    
    def set_efficacy(self, efficacy: float) -> None:
        """
        Set synaptic efficacy.
        
        Args:
            efficacy: Synaptic efficacy
        """
        self._efficacy = efficacy
    
    def step(self, current_time: float) -> None:
        """
        Update synapse for one simulation step.
        
        Args:
            current_time: Current simulation time
        """
        # TODO: Implement real synaptic dynamics
        # For now, just decay eligibility trace
        self._eligibility_trace *= 0.95
        
        # Process transmission queue
        if self._last_transmission_time >= 0:
            # Simple delay model: if delay has passed, transmit
            pass
    
    def reset(self) -> None:
        """Reset synapse to initial state."""
        self._weight = 1.0
        self._delay = 1
        self._type = SynapseType.EXCITATORY
        self._plasticity_flags = PlasticityFlags()
        self._eligibility_trace = 0.0
        self._efficacy = 1.0
        self.clear_history()
        self._last_transmission_time = -1.0
        self._transmission_queue.clear()
    
    def initialize_random(self, rng: RandomGenerator) -> None:
        """
        Initialize synapse with random parameters.
        
        Args:
            rng: Random number generator
        """
        self._weight = rng.uniform_real(0.1, 10.0)
        self._delay = rng.uniform_int(0, 5)
        self._type = random.choice(list(SynapseType))
        self._efficacy = 1.0
        self._eligibility_trace = 0.0
    
    def __repr__(self) -> str:
        """
        String representation of synapse.
        
        Returns:
            String representation
        """
        return f"<Synapse id={self._synapse_id} source={self._source_neuron} dest={self._dest_neuron} weight={self._weight:.3f} type={self._type.value}>"

__all__ = ['Synapse']
