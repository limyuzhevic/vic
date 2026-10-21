"""NLM Neuron class - Leaky Integrate-and-Fire spiking neuron."""

import ctypes
import struct
from enum import IntEnum
from typing import List, Optional

from .enums import NeuronType, NeuronState, FiringState
from .memory import NeuronState as NeuronStateStruct
from .exceptions import NLMException


class Neuron:
    """A single spiking neuron implementing LIF dynamics."""
    
    def __init__(self, neuron_id: int = 0):
        """
        Initialize a neuron.
        
        Args:
            neuron_id: Unique identifier for this neuron
        """
        self._neuron_id = neuron_id
        self._lib = None
        self._neuron_ptr = None
        self._initialized = False
        self._synaptic_input = 0.0
        
    def initialize(self, neuron_type: NeuronType = NeuronType.INTERNAL,
                   resting_potential: float = -70.0,
                   threshold: float = -55.0,
                   refractory_period: int = 5):
        """
        Initialize neuron parameters.
        
        Args:
            neuron_type: Type of neuron
            resting_potential: Membrane resting potential (mV)
            threshold: Spike threshold (mV)
            refractory_period: Refractory period in ms
        """
        if self._initialized:
            raise NLMException("Neuron already initialized")
            
        self._neuron_type = neuron_type
        self._resting_potential = resting_potential
        self._threshold = threshold
        self._refractory_period = refractory_period
        
        # LIF parameters
        self._membrane_capacitance = 1.0  # nF
        self._time_constant = 20.0  # ms
        self._adaptation_strength = 0.1
        
        # State variables
        self._membrane_potential = resting_potential
        self._firing_state = FiringState.RESTING
        self._refractory_remaining = 0
        self._adaptation_variable = 0.0
        self._last_spike_time = -1.0
        self._firing_rate = 0.0
        
        # History
        self._spike_history = []
        self._input_history = []
        
        self._initialized = True
        
    def step(self, current_time: float, dt: float = 0.001):
        """
        Advance neuron dynamics by one timestep.
        
        Args:
            current_time: Current simulation time (ms)
            dt: Time step duration (ms)
        """
        if not self._initialized:
            raise NLMException("Neuron not initialized")
            
        # Handle refractory period
        if self._refractory_remaining > 0:
            self._refractory_remaining -= dt
            if self._refractory_remaining <= 0:
                self._firing_state = FiringState.RESTING
            return
        
        # LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
        leak_contribution = (self._resting_potential - self._membrane_potential) / self._time_constant
        
        # Calculate synaptic input (will be set by external sources)
        synaptic_input = self._synaptic_input / self._membrane_capacitance
        
        # Update membrane potential
        self._membrane_potential += dt * 1000 * (leak_contribution + synaptic_input)
        
        # Apply spike-frequency adaptation
        if self._adaptation_variable > 0:
            self._membrane_potential -= self._adaptation_variable * self._adaptation_strength
            self._adaptation_variable *= 0.95  # Decay adaptation
        
        # Clamp membrane potential
        self._membrane_potential = max(-100.0, min(50.0, self._membrane_potential))
        
        # Check for spike
        if self._membrane_potential >= self._threshold:
            self._fire(current_time)
        
        # Update firing rate estimate
        self._firing_rate = self._calculate_firing_rate(current_time)
        
    def _fire(self, current_time: float):
        """Handle spike event."""
        self._firing_state = FiringState.ACTIVE
        self._last_spike_time = current_time
        self._spike_history.append(current_time)
        
        # Reset membrane potential
        self._membrane_potential = self._resting_potential - 15.0  # Reset below threshold
        
        # Enter refractory period
        self._refractory_remaining = self._refractory_period
        self._firing_state = FiringState.REFRACTORY
        
        # Update adaptation
        self._adaptation_variable += 1.0
        
    def inject_current(self, current: float):
        """
        Inject current into neuron.
        
        Args:
            current: Current amplitude (nA)
        """
        if not self._initialized:
            raise NLMException("Neuron not initialized")
            
        self._synaptic_input += current
        
    def record_spike(self, timestamp: float):
        """
        Record external spike input.
        
        Args:
            timestamp: Spike timestamp (ms)
        """
        self._spike_history.append(timestamp)
        
    def reset(self):
        """Reset neuron to initial state."""
        if not self._initialized:
            return
            
        self._membrane_potential = self._resting_potential
        self._firing_state = FiringState.RESTING
        self._refractory_remaining = 0
        self._adaptation_variable = 0.0
        self._last_spike_time = -1.0
        self._firing_rate = 0.0
        self._spike_history.clear()
        self._input_history.clear()
        
    def check_threshold(self) -> bool:
        """Check if neuron has reached threshold."""
        return self._membrane_potential >= self._threshold
        
    def get_membrane_potential(self) -> float:
        """Get current membrane potential."""
        return self._membrane_potential
        
    def get_threshold(self) -> float:
        """Get threshold potential."""
        return self._threshold
        
    def get_type(self) -> NeuronType:
        """Get neuron type."""
        return self._neuron_type
        
    def get_state(self) -> FiringState:
        """Get firing state."""
        return self._firing_state
        
    def get_last_spike_time(self) -> float:
        """Get last spike time."""
        return self._last_spike_time
        
    def get_firing_rate(self) -> float:
        """Get firing rate (Hz)."""
        return self._firing_rate
        
    def get_spike_history(self) -> List[float]:
        """Get spike history."""
        return self._spike_history.copy()
        
    def get_id(self) -> int:
        """Get neuron ID."""
        return self._neuron_id
        
    def _calculate_firing_rate(self, current_time: float) -> float:
        """Calculate instantaneous firing rate from recent spikes."""
        if len(self._spike_history) < 2:
            return 0.0
            
        # Count spikes in last 100ms
        recent_spikes = [t for t in self._spike_history if (current_time - t) <= 100.0]
        if len(recent_spikes) < 2:
            return 0.0
            
        # Simple rate calculation
        time_window = max(1.0, recent_spikes[-1] - recent_spikes[0])
        return (len(recent_spikes) - 1) / (time_window / 1000.0)  # Convert to Hz


# Factory function
def create_neuron(neuron_id: int = 0) -> Neuron:
    """Create a new neuron."""
    return Neuron(neuron_id)