from dataclasses import dataclass, field
"""
Neuron Dynamics - Leaky Integrate-and-Fire Model
==================================================

Implementation of biologically plausible neuron models with
spike-frequency adaptation, refractory periods, and synaptic integration.

This module provides the fundamental building blocks for neural computation
in the NLM framework, implementing the Leaky Integrate-and-Fire (LIF) model
which is widely used for simulating neural dynamics due to its balance
of biological plausibility and computational efficiency.

Key Features:
- Biologically realistic membrane potential dynamics
- Spike-frequency adaptation through adaptation variable
- Refractory periods after spikes
- Synaptic input integration
- Multiple neuron types (excitatory, inhibitory, modulatory, sensory, motor, internal)
- Firing state management (resting, active, refractory, inhibited)
- Spike history tracking for plasticity

Mathematical Background:
The LIF model follows the differential equation:
dV/dt = (V_rest - V)/tau + I/C
where:
- V is membrane potential
- V_rest is resting potential
- tau is time constant
- I is total injected current
- C is membrane capacitance

Spike detection occurs when V reaches threshold, followed by reset
and refractory period.

Attributes:
    NEURON_TYPES (List[str]): Valid neuron type strings
    FIRING_STATES (List[str]): Valid firing state strings

Classes:
    NeuronType: Enumeration of neuron types
    FiringState: Enumeration of firing states
    NeuronState: Dataclass for neuron membrane dynamics
    Neuron: Main LIF neuron implementation
"""

from typing import List, Dict, Any, Optional
from dataclasses import dataclass, field
import math
# Neuron type enumeration
class NeuronType:
    """Neuron type enumeration representing different functional roles in neural circuits.

    Neuron types determine the functional role and behavior of neurons in the simulation:
    - EXCITATORY: Promotes depolarization and firing in postsynaptic neurons
    - INHIBITORY: Hyperpolarizes and suppresses firing in postsynaptic neurons
    - MODULATORY: Provides gain control and influences synaptic efficacy
    - SENSORY: Receives external sensory inputs
    - MOTOR: Executes actions and produces outputs
    - INTERNAL: Integrates and processes internal signals

    Each neuron type has distinct synaptic properties and response characteristics
    that shape network dynamics and information flow.

    Attributes:
        EXCITATORY (str): Neuron type for excitatory neurons
        INHIBITORY (str): Neuron type for inhibitory neurons
        MODULATORY (str): Neuron type for modulatory neurons
        SENSORY (str): Neuron type for sensory neurons
        MOTOR (str): Neuron type for motor neurons
        INTERNAL (str): Neuron type for internal/cortical neurons

    Example:
        >>> from pynlm.neurons import NeuronType
        >>> 
        >>> # Check neuron type
        >>> neuron = Neuron(0, NeuronType.EXCITATORY)
        >>> print(neuron.neuron_type)
        Excitatory
        >>> 
        >>> # Convert string to neuron type
        >>> neuron_type = NeuronType.from_string("sensory")
        >>> print(neuron_type)
        Sensory
    """
    
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    SENSORY = "Sensory"
    MOTOR = "Motor"
    INTERNAL = "Internal"
    
    # For backward compatibility
    @classmethod
    def values(cls):
        """Get all neuron type values.

        Returns:
            List[str]: List of all neuron type strings
        """
        return [cls.EXCITATORY, cls.INHIBITORY, cls.MODULATORY, 
               cls.SENSORY, cls.MOTOR, cls.INTERNAL]
        
    @classmethod
    def from_string(cls, value: str):
        """Convert string to NeuronType.

        Args:
            value (str): String representation of neuron type

        Returns:
            str: Neuron type string

        Example:
            >>> NeuronType.from_string("excitatory")
            'Excitatory'
            >>> NeuronType.from_string("unknown")
            'Internal'
        """
        for neuron_type in cls.values():
            if neuron_type.lower() == value.lower():
                return neuron_type
        return cls.INTERNAL
        
    @classmethod
    def to_string(cls, neuron_type):
        """Convert NeuronType to string.

        Args:
            neuron_type (str): Neuron type

        Returns:
            str: Neuron type string
        """
        return neuron_type
# Firing state enumeration
class FiringState:
    """Neuron firing state enumeration representing the current functional state of a neuron.

    Firing states capture the dynamic behavior of neurons between spikes:
    - RESTING: Neuron at resting potential, not actively firing
    - ACTIVE: Neuron depolarized and capable of firing
    - REFRACTORY: Neuron recovering from recent spike, unable to fire
    - INHIBITED: Neuron actively suppressed by inhibitory inputs

    The state transitions are determined by membrane potential dynamics,
    synaptic inputs, and internal neuronal properties.

    Attributes:
        RESTING (str): Neuron is at rest
        ACTIVE (str): Neuron is active/depolarized
        REFRACTORY (str): Neuron is in refractory period
        INHIBITED (str): Neuron is inhibited

    Example:
        >>> from pynlm.neurons import FiringState
        >>> 
        >>> # Check firing state
        >>> neuron = Neuron(0)
        >>> print(neuron.state.firing_state)
        Resting
        >>> 
        >>> # Convert string to firing state
        >>> state = FiringState.from_string("active")
        >>> print(state)
        Active
    """
    
    RESTING = "Resting"
    ACTIVE = "Active"
    REFRACTORY = "Refractory"
    INHIBITED = "Inhibited"
    
    # For backward compatibility
    @classmethod
    def values(cls):
        """Get all firing state values.

        Returns:
            List[str]: List of all firing state strings
        """
        return [cls.RESTING, cls.ACTIVE, cls.REFRACTORY, cls.INHIBITED]
        
    @classmethod
    def from_string(cls, value: str):
        """Convert string to FiringState.

        Args:
            value (str): String representation of firing state

        Returns:
            str: Firing state string

        Example:
            >>> FiringState.from_string("refractory")
            'Refractory'
            >>> FiringState.from_string("unknown")
            'Resting'
        """
        for firing_state in cls.values():
            if firing_state.lower() == value.lower():
                return firing_state
        return cls.RESTING
        
    @classmethod
    def to_string(cls, firing_state):
        """Convert FiringState to string.

        Args:
            firing_state (str): Firing state

        Returns:
            str: Firing state string
        """
        return firing_state
@dataclass
class NeuronState:
    """Neuron state for LIF dynamics.

    This dataclass encapsulates the complete state of a neuron for the
    Leaky Integrate-and-Fire model. It includes membrane potential dynamics,
    activity states, adaptation mechanisms, and synaptic inputs.

    The state variables determine neuron behavior and are updated at each
    simulation step according to the LIF differential equation.

    Attributes:
        membrane_potential (float): Current membrane potential (mV)
        resting_potential (float): Resting membrane potential (mV)
        threshold (float): Firing threshold (mV)
        reset_potential (float): Post-spike reset value (mV)
        leak_conductance (float): Membrane leak conductance (nS)
        firing_rate (float): Current firing rate (Hz)
        firing_state (str): Current firing state
        refractory_remaining (int): Steps remaining in refractory period
        refractory_period (int): Total refractory period (steps)
        adaptation_variable (float): Spike-frequency adaptation current
        last_spike_time (float): Last spike timestamp
        synaptic_input (float): Accumulated synaptic input current
        total_current (float): Total injected current

    Example:
        >>> from pynlm.neurons import NeuronState
        >>> 
        >>> state = NeuronState()
        >>> print(f"Membrane potential: {state.membrane_potential} mV")
        >>> print(f"Firing threshold: {state.threshold} mV")
        >>> 
        >>> # Modify state
        >>> state.membrane_potential = -60.0
        >>> state.firing_state = "Active"
        >>> state.adaptation_variable = 0.5

    Note:
        All potential values are in millivolts (mV). Time constants are
        in milliseconds (ms) unless otherwise specified.
    """
    
    # Membrane potential dynamics
    membrane_potential: float = -70.0  # mV
    resting_potential: float = -70.0   # mV
    threshold: float = -55.0           # mV
    reset_potential: float = -70.0     # mV
    leak_conductance: float = 10.0     # nS
    
    # Activity dynamics
    firing_rate: float = 0.0           # Hz
    firing_state: str = "Resting"      # Resting, Active, Refractory, Inhibited
    refractory_remaining: int = 0      # Steps remaining
    refractory_period: int = 5         # Total refractory period
    
    # Adaptation
    adaptation_variable: float = 0.0   # Spike-frequency adaptation
    last_spike_time: float = -1.0      # Last spike timestamp
    
    # Synaptic inputs
    synaptic_input: float = 0.0        # Accumulated synaptic current
    total_current: float = 0.0         # Total injected current
class Neuron:
    """
    Leaky Integrate-and-Fire (LIF) neuron model.

    Biologically plausible neuron dynamics with:
    - Membrane potential integration with leak
    - Spike detection and refractory periods
    - Spike-frequency adaptation
    - Synaptic input integration
    - Adaptive firing rates

    The LIF neuron is a simplified but effective model of neuronal dynamics
    that captures essential features of real neurons while remaining
    computationally efficient. It implements the classic integrate-and-fire
    model with added biological realism through adaptation mechanisms.

    Neuron Behavior:
    1. Membrane potential integrates inputs with exponential decay
    2. When threshold is reached, neuron spikes
    3. Spike triggers refractory period and adaptation
    4. Membrane potential resets and recovers
    5. Synaptic inputs are integrated and contribute to dynamics

    Attributes:
        neuron_id (int): Unique neuron identifier
        neuron_type (str): Neuron type (excitatory, inhibitory, modulatory, etc.)
        region_id (int): Brain region ID (for multi-region simulations)
        population_id (int): Neural population ID (for population modeling)
        state (NeuronState): Current neuron state
        plasticity_flags (Dict[str, bool]): Enabled plasticity mechanisms
        spike_history (List[float]): History of spike times

    Example:
        >>> from pynlm.neurons import Neuron, NeuronType
        >>> 
        >>> # Create neuron
        >>> neuron = Neuron(neuron_id=0, neuron_type=NeuronType.EXCITATORY)
        >>> print(neuron)
        <Neuron id=0 type=Excitatory state=Resting>
        >>> 
        >>> # Set parameters
        >>> neuron.set_parameters({"threshold": -50.0, "adaptation_variable": 0.1})
        >>> 
        >>> # Update neuron state
        >>> neuron.update_parameters(dt=0.001, current_time=0.0)
        >>> 
        >>> # Check state
        >>> state_dict = neuron.get_state_dict()
        >>> print(f"Membrane potential: {state_dict['membrane_potential']} mV")

    Note:
        This is a fundamental neuron model. For complex network simulations,
        consider using the Brain class which manages neuron populations and
        their interactions.
    """

    # Neuron types
    NEURON_TYPES = ["Excitatory", "Inhibitory", "Modulatory", "Sensory", "Motor", "Internal"]

    # Firing states
    FIRING_STATES = ["Resting", "Active", "Refractory", "Inhibited"]

    def __init__(self, neuron_id: int, neuron_type: str = "Internal"):
        """
        Initialize neuron.

        Args:
            neuron_id (int): Unique neuron identifier
            neuron_type (str): Neuron type

        Example:
            >>> neuron = Neuron(0, "Excitatory")
            >>> neuron = Neuron(neuron_id=1, neuron_type="Sensory")

        Note:
            The neuron_id should be unique within a neural population.
            Neuron types determine synaptic properties and response characteristics.
        """
        self.neuron_id = neuron_id
        self.neuron_type = neuron_type
        self.region_id = None
        self.population_id = None
        
        # Initialize state
        self.state = NeuronState()
        
        # Synaptic connections
        self.incoming_synapses = []  # List of (synapse_id, weight, type)
        self.outgoing_synapses = []   # List of (synapse_id, weight, type)
        
        # Plasticity flags
        self.plasticity_flags = {
            "hebbian": True,
            "stdp": True,
            "reward_modulated": False
        }
        
        # Spike history
        self.spike_history = []
        
        # Constants
        self._membrane_capacitance = 1.0    # nF
        self._time_constant = 20.0           # ms
        self._max_spike_history = 100
        
    def set_parameters(self, params: Dict[str, Any]):
        """
        Set neuron parameters.

        Args:
            params (Dict[str, Any]): Dictionary of neuron parameters

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.set_parameters({
            ...     "threshold": -50.0,
            ...     "adaptation_variable": 0.5,
            ...     "refractory_period": 10
            ... })

        Note:
            Only parameters that exist as attributes of the NeuronState
            dataclass will be set. Unknown parameters are ignored.
        """
        for key, value in params.items():
            if hasattr(self.state, key):
                setattr(self.state, key, value)
                
    def get_state_dict(self) -> Dict[str, Any]:
        """
        Get neuron state as dictionary.

        Returns:
            Dict[str, Any]: Neuron state dictionary with the following keys:
                - id (int): Neuron identifier
                - type (str): Neuron type
                - region_id (Optional[int]): Brain region ID
                - population_id (Optional[int]): Neural population ID
                - membrane_potential (float): Current membrane potential
                - threshold (float): Firing threshold
                - firing_state (str): Current firing state
                - firing_rate (float): Current firing rate
                - refractory_remaining (int): Steps remaining in refractory period
                - last_spike_time (float): Last spike timestamp
                - plasticity_flags (Dict[str, bool]): Plasticity mechanism flags
                - spike_count (int): Number of spikes in history

        Example:
            >>> neuron = Neuron(0)
            >>> state_dict = neuron.get_state_dict()
            >>> print(f"Neuron {state_dict['id']}: "
            ...       f"Type={state_dict['type']}, "
            ...       f"Firing={state_dict['firing_state']}, "
            ...       f"Spikes={state_dict['spike_count']}")
        """
        return {
            "id": self.neuron_id,
            "type": self.neuron_type,
            "region_id": self.region_id,
            "population_id": self.population_id,
            "membrane_potential": self.state.membrane_potential,
            "threshold": self.state.threshold,
            "firing_state": self.state.firing_state,
            "firing_rate": self.state.firing_rate,
            "refractory_remaining": self.state.refractory_remaining,
            "last_spike_time": self.state.last_spike_time,
            "plasticity_flags": self.plasticity_flags,
            "spike_count": len(self.spike_history)
        }
        
    def update_parameters(self, dt: float, current_time: float):
        """
        Update neuron state for one time step.

        This method implements the core LIF dynamics, updating the neuron's
        membrane potential based on synaptic inputs, leak currents, and
        adaptation mechanisms.

        Args:
            dt (float): Time step duration in milliseconds
            current_time (float): Current simulation time in milliseconds

        Example:
            >>> neuron = Neuron(0)
            >>> # Simulate for 10ms
            >>> for t in np.arange(0, 10, 0.1):
            ...     neuron.update_parameters(dt=0.1, current_time=t)
            ...     print(f"t={t:.1f}ms, V={neuron.state.membrane_potential:.2f}mV")

        Note:
            The time step dt should be small enough to accurately capture
            membrane potential dynamics (typically 0.1-1.0 ms).
        """
        # Handle refractory period
        if self.state.refractory_remaining > 0:
            self.state.refractory_remaining -= 1
            if self.state.refractory_remaining == 0:
                self.state.firing_state = "Resting"
            self.state.synaptic_input = 0.0
            return
            
        # LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
        V = self.state.membrane_potential
        V_rest = self.state.resting_potential
        tau = self._time_constant
        C = self._membrane_capacitance
        
        # Synaptic input contribution
        synaptic_contribution = self.state.synaptic_input / C
        
        # Leak contribution
        leak_contribution = (V_rest - V) / tau
        
        # Update membrane potential
        V_new = V + dt * (leak_contribution + synaptic_contribution)
        
        # Spike-frequency adaptation
        if self.state.adaptation_variable > 0.0:
            V_new -= self.state.adaptation_variable * 0.01
            self.state.adaptation_variable *= 0.95
            
        # Clamp membrane potential
        V_new = max(-100.0, min(50.0, V_new))
        
        # Check for spike
        if V_new >= self.state.threshold:
            self._handle_spike(current_time)
            V_new = self.state.reset_potential
            self.state.firing_state = "Refractory"
            self.state.refractory_remaining = self.state.refractory_period
            self.state.adaptation_variable += 1.0
        else:
            self.state.firing_state = "Active"
            
        self.state.membrane_potential = V_new
        
        # Clear synaptic input
        self.state.synaptic_input = 0.0
        
    def _handle_spike(self, current_time: float):
        """
        Handle neuron spike event.

        This method is called when the neuron reaches firing threshold.
        It records the spike time and updates adaptation variables.

        Args:
            current_time (float): Current simulation time in milliseconds

        Note:
            This method is called internally by update_parameters().
            Users typically don't call this directly.
        """
        self.state.last_spike_time = current_time
        self.spike_history.append(current_time)
        
        # Keep only recent spikes
        if len(self.spike_history) > self._max_spike_history:
            self.spike_history.pop(0)
            
    def receive_excitatory_input(self, amplitude: float):
        """
        Receive excitatory synaptic input.

        Args:
            amplitude (float): Input amplitude (positive for excitation)

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.receive_excitatory_input(0.5)  # Strong excitation
            >>> neuron.receive_excitatory_input(0.1)  # Weak excitation

        Note:
            Excitatory inputs increase membrane potential, bringing the
            neuron closer to firing threshold.
        """
        self.state.synaptic_input += amplitude
        
    def receive_inhibitory_input(self, amplitude: float):
        """
        Receive inhibitory synaptic input.

        Args:
            amplitude (float): Input amplitude (negative for inhibition)

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.receive_inhibitory_input(-0.5)  # Strong inhibition
            >>> neuron.receive_inhibitory_input(-0.1)  # Weak inhibition

        Note:
            Inhibitory inputs decrease membrane potential, pushing the
            neuron away from firing threshold.
        """
        self.state.synaptic_input -= amplitude
        
    def receive_modulatory_input(self, amplitude: float):
        """
        Receive modulatory input.

        Args:
            amplitude (float): Modulatory input amplitude

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.receive_modulatory_input(0.2)  # Increase adaptation

        Note:
            Modulatory inputs affect adaptation variables and other
            non-linear mechanisms that shape firing patterns.
        """
        self.state.adaptation_variable += amplitude * 0.1
        
    def inject_current(self, current: float):
        """
        Inject external current into neuron.

        Args:
            current (float): Current amplitude (positive for depolarization)

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.inject_current(1.0)  # Inject depolarizing current
            >>> neuron.inject_current(-0.5) # Inject hyperpolarizing current

        Note:
            External current injection is useful for controlling neuron
            firing patterns in simulations and experiments.
        """
        self.state.synaptic_input += current
        
    def add_incoming_synapse(self, synapse_id: int, weight: float, syn_type: str):
        """
        Add incoming synapse.

        Args:
            synapse_id (int): Synapse identifier
            weight (float): Synaptic weight
            syn_type (str): Synapse type

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.add_incoming_synapse(0, 0.8, "excitatory")
            >>> neuron.add_incoming_synapse(1, -0.5, "inhibitory")

        Note:
            Synapses represent connections from other neurons. The weight
            determines the strength of the connection.
        """
        self.incoming_synapses.append({
            "id": synapse_id,
            "weight": weight,
            "type": syn_type
        })
        
    def add_outgoing_synapse(self, synapse_id: int, weight: float, syn_type: str):
        """
        Add outgoing synapse.

        Args:
            synapse_id (int): Synapse identifier
            weight (float): Synaptic weight
            syn_type (str): Synapse type

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.add_outgoing_synapse(0, 0.8, "excitatory")
            >>> neuron.add_outgoing_synapse(1, 0.3, "inhibitory")

        Note:
            Outgoing synapses represent connections to other neurons. This
            method is called when creating synaptic connections between neurons.
        """
        self.outgoing_synapses.append({
            "id": synapse_id,
            "weight": weight,
            "type": syn_type
        })
        
    def enable_plasticity(self, hebbian: bool = True, stdp: bool = True, 
                          reward_modulated: bool = False):
        """
        Enable plasticity mechanisms.

        Args:
            hebbian (bool): Enable Hebbian plasticity
            stdp (bool): Enable STDP
            reward_modulated (bool): Enable reward-modulated plasticity

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.enable_plasticity(hebbian=True, stdp=True, reward_modulated=True)
            >>> neuron.enable_plasticity(hebbian=False)  # Disable Hebbian only

        Note:
            Plasticity mechanisms determine how synaptic weights change over
            time based on neural activity. Different mechanisms capture
            different biological learning processes.
        """
        self.plasticity_flags["hebbian"] = hebbian
        self.plasticity_flags["stdp"] = stdp
        self.plasticity_flags["reward_modulated"] = reward_modulated
        
    def reset(self):
        """
        Reset neuron to initial state.

        This method resets all neuron state variables to their initial values,
        effectively preparing the neuron for a new simulation.

        Example:
            >>> neuron = Neuron(0)
            >>> neuron.update_parameters(dt=0.1, current_time=0.0)
            >>> # Neuron state has changed...
            >>> 
            >>> neuron.reset()
            >>> # Neuron is back to initial state
            >>> print(neuron.state.firing_state)
            'Resting'

        Note:
            Reset clears the spike history and restores initial state.
            This is useful for starting new simulation runs or experiments.
        """
        self.state = NeuronState()
        self.spike_history.clear()
        
    def __repr__(self) -> str:
        return f"<Neuron id={self.neuron_id} type={self.neuron_type} state={self.state.firing_state}>"
