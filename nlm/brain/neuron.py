"""
Neuron implementation for NLM.
"""

from typing import List, Optional
from dataclasses import dataclass
from ..types import *
from ..random import RandomGenerator

class Neuron:
    """
    Single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.
    
    Represents a biological neuron with LIF dynamics, synaptic input,
    spike generation, and plasticity mechanisms.
    """
    
    def __init__(self, neuron_id: int):
        """
        Initialize neuron with unique ID.
        
        Args:
            neuron_id: Unique identifier for the neuron
        """
        self._neuron_id = neuron_id
        self._type = NeuronType.INTERNAL
        self._region_id = INVALID_REGION_ID
        self._population_id = INVALID_POPULATION_ID
        self._state = NeuronState()
        self._synaptic_input = 0.0
        self._spike_history: List[float] = []
        self._incoming_synapses: List[int] = []
        self._outgoing_synapses: List[int] = []
        self._plasticity_flags = PlasticityFlags()
    
    def get_id(self) -> int:
        """
        Get neuron identifier.
        
        Returns:
            Neuron ID
        """
        return self._neuron_id
    
    def get_type(self) -> NeuronType:
        """
        Get neuron type.
        
        Returns:
            Neuron type
        """
        return self._type
    
    def set_type(self, neuron_type: NeuronType) -> None:
        """
        Set neuron type.
        
        Args:
            neuron_type: Type to set
        """
        self._type = neuron_type
    
    def get_state(self) -> NeuronState:
        """
        Get neuron state.
        
        Returns:
            Neuron state
        """
        return self._state
    
    def set_membrane_potential(self, potential: float) -> None:
        """
        Set membrane potential.
        
        Args:
            potential: Membrane potential in mV
        """
        self._state.membrane_potential = potential
    
    def get_membrane_potential(self) -> float:
        """
        Get membrane potential.
        
        Returns:
            Membrane potential in mV
        """
        return self._state.membrane_potential
    
    def add_to_membrane_potential(self, delta: float) -> None:
        """
        Add delta to membrane potential.
        
        Args:
            delta: Potential change in mV
        """
        self._state.membrane_potential += delta
    
    def get_threshold(self) -> float:
        """
        Get firing threshold.
        
        Returns:
            Firing threshold in mV
        """
        return self._state.threshold
    
    def set_threshold(self, threshold: float) -> None:
        """
        Set firing threshold.
        
        Args:
            threshold: Firing threshold in mV
        """
        self._state.threshold = threshold
    
    def is_firing(self) -> bool:
        """
        Check if neuron is currently firing.
        
        Returns:
            True if firing, False otherwise
        """
        return self._state.firing_state == FiringState.ACTIVE
    
    def is_refractory(self) -> bool:
        """
        Check if neuron is in refractory period.
        
        Returns:
            True if in refractory period, False otherwise
        """
        return self._state.refractory_remaining > 0
    
    def set_firing_state(self, state: FiringState) -> None:
        """
        Set firing state.
        
        Args:
            state: Firing state
        """
        self._state.firing_state = state
    
    def set_refractory_period(self, steps: int) -> None:
        """
        Set refractory period length.
        
        Args:
            steps: Number of simulation steps
        """
        self._state.refractory_period = steps
        self._state.refractory_remaining = steps
    
    def decrement_refractory(self) -> None:
        """Decrement refractory counter."""
        if self._state.refractory_remaining > 0:
            self._state.refractory_remaining -= 1
            if self._state.refractory_remaining == 0:
                self._state.firing_state = FiringState.RESTING
    
    def get_firing_rate(self) -> float:
        """
        Get firing rate.
        
        Returns:
            Firing rate in Hz
        """
        return self._state.firing_rate
    
    def set_firing_rate(self, rate: float) -> None:
        """
        Set firing rate.
        
        Args:
            rate: Firing rate in Hz
        """
        self._state.firing_rate = rate
    
    def set_leak_conductance(self, conductance: float) -> None:
        """
        Set leak conductance.
        
        Args:
            conductance: Leak conductance in nS
        """
        self._state.leak_conductance = conductance
    
    def get_leak_conductance(self) -> float:
        """
        Get leak conductance.
        
        Returns:
            Leak conductance in nS
        """
        return self._state.leak_conductance
    
    def get_refractory_period(self) -> int:
        """
        Get refractory period length.
        
        Returns:
            Refractory period in simulation steps
        """
        return self._state.refractory_period
    
    def set_resting_potential(self, potential: float) -> None:
        """
        Set resting potential.
        
        Args:
            potential: Resting potential in mV
        """
        self._state.resting_potential = potential
    
    def get_resting_potential(self) -> float:
        """
        Get resting potential.
        
        Returns:
            Resting potential in mV
        """
        return self._state.resting_potential
    
    def set_reset_potential(self, potential: float) -> None:
        """
        Set reset potential.
        
        Args:
            potential: Reset potential in mV
        """
        self._state.reset_potential = potential
    
    def check_threshold(self) -> bool:
        """
        Check if membrane potential exceeds threshold.
        
        Returns:
            True if threshold exceeded, False otherwise
        """
        return self._state.membrane_potential >= self._state.threshold
    
    def get_last_spike_time(self) -> float:
        """
        Get time of last spike.
        
        Returns:
            Last spike time, or -1 if no spikes
        """
        return self._state.last_spike_time
    
    def receive_excitatory_input(self, amplitude: float) -> None:
        """
        Receive excitatory synaptic input.
        
        Args:
            amplitude: Synaptic conductance strength
        """
        self._synaptic_input += amplitude
    
    def receive_inhibitory_input(self, amplitude: float) -> None:
        """
        Receive inhibitory synaptic input.
        
        Args:
            amplitude: Synaptic conductance strength
        """
        self._synaptic_input -= amplitude
    
    def receive_modulatory_input(self, amplitude: float) -> None:
        """
        Receive modulatory input.
        
        Args:
            amplitude: Modulatory signal strength
        """
        self._state.adaptation_variable += amplitude * 0.1
    
    def inject_current(self, current: float) -> None:
        """
        Inject current directly into neuron.
        
        Args:
            current: Current in nA
        """
        self._synaptic_input += current
    
    def get_total_current(self) -> float:
        """
        Get total synaptic current.
        
        Returns:
            Total synaptic current
        """
        return self._synaptic_input
    
    def clear_total_current(self) -> None:
        """Clear synaptic input."""
        self._synaptic_input = 0.0
    
    def record_spike(self, timestamp: float) -> None:
        """
        Record a spike at given time.
        
        Args:
            timestamp: Time of spike
        """
        self._spike_history.append(timestamp)
        self._state.spike_count += 1
        if len(self._spike_history) > 100:
            self._spike_history.pop(0)
    
    def get_spike_history(self) -> List[float]:
        """
        Get spike history.
        
        Returns:
            List of spike times
        """
        return self._spike_history.copy()
    
    def clear_spike_history(self) -> None:
        """Clear spike history."""
        self._spike_history.clear()
        self._state.spike_count = 0
    
    def add_incoming_synapse(self, synapse_handle: int) -> None:
        """
        Add incoming synapse connection.
        
        Args:
            synapse_handle: Synapse handle
        """
        self._incoming_synapses.append(synapse_handle)
    
    def add_outgoing_synapse(self, synapse_handle: int) -> None:
        """
        Add outgoing synapse connection.
        
        Args:
            synapse_handle: Synapse handle
        """
        self._outgoing_synapses.append(synapse_handle)
    
    def get_incoming_synapses(self) -> List[int]:
        """
        Get incoming synapses.
        
        Returns:
            List of incoming synapse handles
        """
        return self._incoming_synapses.copy()
    
    def get_outgoing_synapses(self) -> List[int]:
        """
        Get outgoing synapses.
        
        Returns:
            List of outgoing synapse handles
        """
        return self._outgoing_synapses.copy()
    
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
    
    def set_region_id(self, region_id: int) -> None:
        """
        Set region identifier.
        
        Args:
            region_id: Region ID
        """
        self._region_id = region_id
    
    def get_region_id(self) -> int:
        """
        Get region identifier.
        
        Returns:
            Region ID
        """
        return self._region_id
    
    def set_population_id(self, population_id: int) -> None:
        """
        Set population identifier.
        
        Args:
            population_id: Population ID
        """
        self._population_id = population_id
    
    def get_population_id(self) -> int:
        """
        Get population identifier.
        
        Returns:
            Population ID
        """
        return self._population_id
    
    def step_lif(self, current_time: float, dt: float = 0.001) -> bool:
        """
        Step LIF neuron for one simulation time step.
        
        Args:
            current_time: Current simulation time
            dt: Time step duration (default: 1ms)
            
        Returns:
            True if neuron fired, False otherwise
        """
        fired = False
        
        # Handle refractory period
        if self._state.refractory_remaining > 0:
            self._state.refractory_remaining -= 1
            if self._state.refractory_remaining == 0:
                self._state.firing_state = FiringState.RESTING
            self._synaptic_input = 0.0
            return False
        
        # LIF dynamics: Leaky Integrate-and-Fire
        # dV/dt = (V_rest - V)/tau + I/C
        V = self._state.membrane_potential
        V_rest = self._state.resting_potential
        V_reset = self._state.reset_potential
        threshold = self._state.threshold
        tau = 20.0  # ms
        C = 1.0  # nF
        
        # Synaptic contribution
        synaptic_contribution = self._synaptic_input / C
        
        # Leak contribution
        leak_contribution = (V_rest - V) / tau
        
        # Update membrane potential
        V = V + dt * 1000.0 * (leak_contribution + synaptic_contribution)
        
        # Apply spike-frequency adaptation
        if self._state.adaptation_variable > 0.0:
            V -= self._state.adaptation_variable * 0.01
            self._state.adaptation_variable *= 0.95
        
        # Clamp membrane potential
        V = max(-100.0, min(50.0, V))
        
        # Check for spike
        if V >= threshold:
            fired = True
            self._state.firing_state = FiringState.ACTIVE
            self._state.last_spike_time = current_time
            self.record_spike(current_time)
            V = V_reset
            self._state.refractory_remaining = self._state.refractory_period
            self._state.firing_state = FiringState.REFRACTORY
            self._state.adaptation_variable += 1.0
        else:
            self._state.firing_state = FiringState.ACTIVE
        
        # Update membrane potential
        self._state.membrane_potential = V
        
        # Clear synaptic input
        self._synaptic_input = 0.0
        
        return fired
    
    def step(self, current_time: float, dt: float = 0.001) -> bool:
        """
        Step neuron for one simulation time step.
        
        Args:
            current_time: Current simulation time
            dt: Time step duration (default: 1ms)
            
        Returns:
            True if neuron fired, False otherwise
        """
        return self.step_lif(current_time, dt)
    
    def reset(self) -> None:
        """Reset neuron to initial state."""
        self._state = NeuronState()
        self._synaptic_input = 0.0
        self.clear_spike_history()
    
    def initialize_random(self, rng: RandomGenerator) -> None:
        """
        Initialize neuron with random parameters.
        
        Args:
            rng: Random number generator
        """
        self._state.membrane_potential = (
            self._state.resting_potential + 
            rng.uniform_real(-3.0, 3.0)
        )
        self._state.threshold = -55.0 + rng.uniform_real(-2.0, 2.0)
        self._state.resting_potential = -70.0 + rng.uniform_real(-2.0, 2.0)
        self._state.reset_potential = (
            self._state.resting_potential + rng.uniform_real(0.0, 5.0)
        )
        self._state.refractory_period = rng.uniform_int(2, 10)
        self._state.firing_state = FiringState.RESTING
        self._state.refractory_remaining = 0
        self._state.adaptation_variable = 0.0
        self._state.last_spike_time = -1.0
        self.clear_total_current()
        self.clear_spike_history()
    
    def __repr__(self) -> str:
        """
        String representation of neuron.
        
        Returns:
            String representation
        """
        return f"<Neuron id={self._neuron_id} type={self._type.value} V={self._state.membrane_potential:.1f}mV threshold={self._state.threshold:.1f}mV firing={'yes' if self.is_firing() else 'no'}>"

__all__ = ['Neuron']
