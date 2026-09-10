// NLM - Python Neural Learning Machine Core Module

import numpy as np
from typing import List, Optional, Dict, Any, Union
import json

# Forward references for type checking
class Brain:
    """Placeholder for Brain class."""
    pass

class ConfigManager:
    """Python wrapper for NLM configuration management."""
    
    def __init__(self):
        self._config = {}
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a JSON file."""
        try:
            with open(filepath, 'r') as f:
                self._config = json.load(f)
            return True
        except Exception:
            return False
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value."""
        self._config[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        return self._config.get(key, default)
    
    def summary(self) -> str:
        """Get a summary of the configuration."""
        return f"Config with {len(self._config)} entries"
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._config.keys())


class NeuralNetwork:
    """High-level neural network interface for NLM."""
    
    def __init__(self, config: Optional[ConfigManager] = None):
        if config is None:
            self._config = ConfigManager()
        else:
            self._config = config
        
        # Initialize brain with default parameters
        self._brain = self._initialize_brain()
    
    def _initialize_brain(self) -> Dict[str, Any]:
        """Initialize brain with parameters from config."""
        return {
            'neuron_count': self._config.get('brain.neuron_count', 1000),
            'synapse_density': self._config.get('brain.synapse_density', 0.1),
            'connection_probability': self._config.get('brain.connection_probability', 0.05),
            'v_thresh': self._config.get('brain.v_thresh', -50.0),
            'v_rest': self._config.get('brain.v_rest', -70.0),
            'v_reset': self._config.get('brain.v_reset', -75.0),
            'tau_mem': self._config.get('brain.tau_mem', 20.0),
            'tau_ref': self._config.get('brain.tau_ref', 2.0),
            'random_seed': self._config.get('random_seed', 42),
            'simulation_timestep': self._config.get('simulation_timestep', 0.001),
            'plasticity': {
                'stdp': {
                    'enable': self._config.get('plasticity.stdp.enable', True),
                    'learning_rate': self._config.get('plasticity.stdp.learning_rate', 0.001),
                    'tau_plus': self._config.get('plasticity.stdp.tau_plus', 20.0),
                    'tau_minus': self._config.get('plasticity.stdp.tau_minus', 20.0)
                },
                'hebbian': {
                    'enable': self._config.get('plasticity.hebbian.enable', True)
                },
                'structural': {
                    'enable': self._config.get('plasticity.structural.enable', True)
                }
            },
            'neuromod': {
                'dopamine': {
                    'scale': self._config.get('neuromod.dopamine.scale', 1.0)
                },
                'curiosity': {
                    'enable': self._config.get('neuromod.curiosity.enable', True)
                },
                'novelty': {
                    'enable': self._config.get('neuromod.novelty.enable', True)
                }
            }
        }
    
    def initialize(self) -> None:
        """Initialize the neural network."""
        # Reset brain state
        self._reset_neurons()
        # Initialize neurons with random parameters
        self._initialize_neurons_randomly()
        # Create synaptic connections
        self._create_synaptic_connections()
    
    def _reset_neurons(self) -> None:
        """Reset all neurons to initial state."""
        for neuron in self._neurons:
            neuron.reset()
    
    def _initialize_neurons_randomly(self) -> None:
        """Initialize neurons with random biological parameters."""
        for neuron in self._neurons:
            neuron.initialize_random()
    
    def _create_synaptic_connections(self) -> None:
        """Create synaptic connections between neurons based on density."""
        num_neurons = len(self._neurons)
        num_connections = int(num_neurons * num_neurons * self._brain['synapse_density'])
        
        for _ in range(num_connections):
            # Random connection with probability
            if np.random.random() < self._brain['connection_probability']:
                source = np.random.randint(0, num_neurons)
                dest = np.random.randint(0, num_neurons)
                if source != dest:
                    self._create_synapse(source, dest)
    
    def _create_synapse(self, source_idx: int, dest_idx: int) -> None:
        """Create a synapse between two neurons."""
        synapse = Synapse(
            source=self._neurons[source_idx],
            destination=self._neurons[dest_idx],
            initial_weight=np.random.uniform(-0.5, 0.5)
        )
        self._neurons[source_idx].add_outgoing_synapse(synapse)
        self._neurons[dest_idx].add_incoming_synapse(synapse)
        self._synapses.append(synapse)
    
    def step(self, step: int, time: float = None) -> None:
        """Perform one simulation step."""
        for neuron in self._neurons:
            neuron.step(time if time is not None else step * self._brain['simulation_timestep'])
        
        # Update plasticity
        self._update_plasticity()
        
        # Update development
        self._update_development()
    
    def _update_plasticity(self) -> None:
        """Update synaptic plasticity rules."""
        for synapse in self._synapses:
            synapse.step()
    
    def _update_development(self) -> None:
        """Update developmental changes."""
        # Simple developmental aging model
        self._development_stage = self._get_development_stage()
    
    def _get_development_stage(self) -> str:
        """Get current developmental stage."""
        total_steps = sum(len(neuron.spike_history) for neuron in self._neurons)
        if total_steps < 100:
            return "initial"
        elif total_steps < 1000:
            return "critical_period"
        elif total_steps < 5000:
            return "maturation"
        else:
            return "adult"
    
    def reset(self) -> None:
        """Reset the neural network."""
        self.initialize()
    
    def get_neuron_count(self) -> int:
        """Get the number of neurons."""
        return len(self._neurons)
    
    def get_synapse_count(self) -> int:
        """Get the number of synapses."""
        return len(self._synapses)
    
    def get_firing_neurons(self) -> int:
        """Get the number of firing neurons."""
        return sum(1 for neuron in self._neurons if neuron.is_firing())
    
    def get_total_spikes(self) -> int:
        """Get the total number of spikes."""
        return sum(len(neuron.spike_history) for neuron in self._neurons)
    
    def get_average_firing_rate(self) -> float:
        """Get the average firing rate."""
        if not self._neurons:
            return 0.0
        total_firing_rate = sum(neuron.firing_rate for neuron in self._neurons)
        return total_firing_rate / len(self._neurons)
    
    def get_developmental_stage(self) -> str:
        """Get the current developmental stage."""
        return self._development_stage
    
    def save(self, filepath: str) -> bool:
        """Save the neural network state."""
        try:
            state = {
                'brain_params': self._brain,
                'neurons': [neuron.get_state() for neuron in self._neurons],
                'synapses': [synapse.get_state() for synapse in self._synapses],
                'development_stage': self._development_stage
            }
            with open(filepath, 'w') as f:
                json.dump(state, f, indent=2)
            return True
        except Exception:
            return False
    
    def load(self, filepath: str) -> bool:
        """Load the neural network state."""
        try:
            with open(filepath, 'r') as f:
                state = json.load(f)
            
            self._brain = state['brain_params']
            self._neurons = [Neuron.from_state(neuron_state) for neuron_state in state['neurons']]
            self._synapses = [Synapse.from_state(synapse_state) for synapse_state in state['synapses']]
            self._development_stage = state['development_stage']
            
            # Reestablish connections
            self._reestablish_synapse_connections()
            
            return True
        except Exception:
            return False
    
    def _reestablish_synapse_connections(self) -> None:
        """Reestablish synaptic connections from saved state."""
        for synapse in self._synapses:
            source = next((n for n in self._neurons if n.id == synapse.source_id), None)
            dest = next((n for n in self._neurons if n.id == synapse.destination_id), None)
            if source and dest:
                source.add_outgoing_synapse(synapse)
                dest.add_incoming_synapse(synapse)


class Synapse:
    """Synaptic connection between neurons."""
    
    def __init__(self, source, destination, initial_weight: float = 0.0):
        self.source = source
        self.destination = destination
        self.weight = initial_weight
        self.initial_weight = initial_weight
        self.delay = 1  # ms
        self.type = "excitatory"  # excitatory, inhibitory, modulatory
        self.plasticity_flags = {
            'hebbian': True,
            'stdp': True,
            'reward_modulated': False
        }
        
        # Short-term plasticity state
        self.short_term_facilitation = 0.0
        self.short_term_depression = 1.0
        self.last_pre_spike_time = -1.0
        self.last_post_spike_time = -1.0
        
        # Learning state
        self.eligibility_trace = 0.0
        self.efficacy = 1.0
        self.release_probability = 0.5
        
        # Historical data for plasticity
        self.pre_spike_history = []
        self.post_spike_history = []
    
    def record_pre_spike(self, timestamp: float) -> None:
        """Record presynaptic spike."""
        self.pre_spike_history.append(timestamp)
        self.last_pre_spike_time = timestamp
        self.eligibility_trace += 1.0  # Increment eligibility trace
    
    def record_post_spike(self, timestamp: float) -> None:
        """Record postsynaptic spike."""
        self.post_spike_history.append(timestamp)
        self.last_post_spike_time = timestamp
    
    def get_weight(self) -> float:
        """Get synaptic weight."""
        return self.weight
    
    def set_weight(self, weight: float) -> None:
        """Set synaptic weight."""
        self.weight = max(-1.0, min(1.0, weight))  # Clamp to [-1, 1]
    
    def add_to_weight(self, delta: float) -> None:
        """Add to synaptic weight."""
        self.set_weight(self.weight + delta)
    
    def get_efficacy(self) -> float:
        """Get synaptic efficacy."""
        return self.efficacy
    
    def set_efficacy(self, efficacy: float) -> None:
        """Set synaptic efficacy."""
        self.efficacy = max(0.0, min(2.0, efficacy))  # Clamp to [0, 2]
    
    def get_eligibility_trace(self) -> float:
        """Get eligibility trace."""
        return self.eligibility_trace
    
    def set_eligibility_trace(self, trace: float) -> None:
        """Set eligibility trace."""
        self.eligibility_trace = trace
    
    def decay_eligibility_trace(self, decay_rate: float) -> None:
        """Decay eligibility trace."""
        self.eligibility_trace *= (1.0 - decay_rate)
        if abs(self.eligibility_trace) < 0.001:
            self.eligibility_trace = 0.0
    
    def step(self) -> None:
        """Update synapse for one simulation step."""
        # Update short-term plasticity
        self._update_short_term_plasticity()
        
        # Apply plasticity rules
        self._apply_plasticity()
        
        # Decay eligibility trace
        self.decay_eligibility_trace(0.001)
    
    def _update_short_term_plasticity(self) -> None:
        """Update short-term facilitation and depression."""
        current_time = len(self.pre_spike_history) + len(self.post_spike_history)
        
        # Decay facilitation
        if self.last_pre_spike_time >= 0:
            time_since_pre = current_time - self.last_pre_spike_time
            self.short_term_facilitation *= np.exp(-time_since_pre / 100.0)
        
        # Recover from depression
        if self.last_post_spike_time >= 0:
            time_since_post = current_time - self.last_post_spike_time
            self.short_term_depression += (1.0 - self.short_term_depression) * (1.0 - np.exp(-time_since_post / 200.0))
        
        # Update efficacy based on STP
        self.efficacy = min(2.0, self.efficacy * self.short_term_facilitation * self.short_term_depression)
    
    def _apply_plasticity(self) -> None:
        """Apply plasticity rules based on spike history."""
        if not self.pre_spike_history or not self.post_spike_history:
            return
        
        # Check for spike timing dependent plasticity (STDP)
        recent_pre = self.pre_spike_history[-1]
        recent_post = self.post_spike_history[-1]
        
        if recent_post > recent_pre:
            # Post spike after pre spike -> Long-Term Potentiation (LTP)
            delta = 0.01 * self.efficacy
            self.add_to_weight(delta)
        elif recent_post < recent_pre:
            # Post spike before pre spike -> Long-Term Depression (LTD)
            delta = -0.01 * self.efficacy
            self.add_to_weight(delta)
        
        # Apply Hebbian learning if enabled
        if self.plasticity_flags['hebbian']:
            # Simple Hebbian learning: correlated activity strengthens synapse
            if self.pre_spike_history and self.post_spike_history:
                correlation = 1.0  # In real implementation, would use spike timing
                hebbian_delta = correlation * 0.005
                self.add_to_weight(hebbian_delta)
        
        # Apply reward-modulated learning if enabled
        if self.plasticity_flags['reward_modulated'] and self.eligibility_trace > 0:
            reward_modulation = self.efficacy * self.eligibility_trace * 0.001
            self.add_to_weight(reward_modulation)
    
    def reset(self) -> None:
        """Reset synapse to initial state."""
        self.weight = self.initial_weight
        self.short_term_facilitation = 0.0
        self.short_term_depression = 1.0
        self.last_pre_spike_time = -1.0
        self.last_post_spike_time = -1.0
        self.eligibility_trace = 0.0
        self.efficacy = 1.0
        self.release_probability = 0.5
        self.pre_spike_history.clear()
        self.post_spike_history.clear()
    
    def initialize_random(self) -> None:
        """Initialize synapse with random biological parameters."""
        # Random initial weight based on type
        if self.type == "excitatory":
            self.weight = np.random.uniform(0.1, 0.4)
        elif self.type == "inhibitory":
            self.weight = np.random.uniform(-0.4, -0.1)
        else:
            self.weight = np.random.uniform(-0.1, 0.1)
        
        self.initial_weight = self.weight
        self.efficacy = np.random.uniform(0.8, 1.0)
        self.short_term_facilitation = 0.0
        self.short_term_depression = 1.0
        self.release_probability = np.random.uniform(0.3, 0.7)
    
    def get_state(self) -> Dict[str, Any]:
        """Get synapse state."""
        return {
            'source_id': self.source.id,
            'destination_id': self.destination.id,
            'weight': self.weight,
            'type': self.type,
            'efficacy': self.efficacy,
            'release_probability': self.release_probability,
            'last_pre_spike_time': self.last_pre_spike_time,
            'last_post_spike_time': self.last_post_spike_time,
            'plasticity_flags': self.plasticity_flags
        }
    
    def from_state(state: Dict[str, Any]) -> 'Synapse':
        """Create synapse from saved state."""
        # This is a simplified implementation
        # In a real implementation, we would need to restore neuron references
        synapse = Synapse(None, None)
        synapse.__dict__.update(state)
        return synapse


class LIFNeuron:
    """Leaky Integrate-and-Fire neuron implementation."""
    
    def __init__(self, neuron_id: int = 0, neuron_type: str = "internal"):
        self.id = neuron_id
        self.type = neuron_type
        self.membrane_potential = -70.0
        self.resting_potential = -70.0
        self.threshold = -55.0
        self.reset_potential = -70.0
        self.leak_conductance = 10.0
        self.refractory_period = 5
        self.refractory_remaining = 0
        self.firing_rate = 0.0
        self.firing_state = "resting"
        self.spike_history = []
        self.current_input = 0.0
        self.adaptation_variable = 0.0
        self.last_spike_time = -1.0
        self.synapses_in = []
        self.synapses_out = []
    
    def is_firing(self) -> bool:
        """Check if neuron is currently firing."""
        return self.firing_state == "active" or self.firing_state == "refractory"
    
    def add_incoming_synapse(self, synapse: Synapse) -> None:
        """Add incoming synapse."""
        self.synapses_in.append(synapse)
    
    def add_outgoing_synapse(self, synapse: Synapse) -> None:
        """Add outgoing synapse."""
        self.synapses_out.append(synapse)
    
    def step(self, current_time: float, dt: float = 0.001) -> bool:
        """Perform one simulation step."""
        # Handle refractory period
        if self.refractory_remaining > 0:
            self.refractory_remaining -= 1
            if self.refractory_remaining == 0:
                self.firing_state = "resting"
            return False
        
        # LIF dynamics
        self._update_membrane_potential(dt)
        
        # Check for spike
        if self.membrane_potential >= self.threshold:
            return self._handle_spike(current_time)
        else:
            self.firing_state = "active"
        
        # Clear synaptic input
        self.current_input = 0.0
        
        return False
    
    def _update_membrane_potential(self, dt: float) -> None:
        """Update membrane potential using LIF dynamics."""
        # LIF equation: dV/dt = (V_rest - V)/tau + I/C
        # Euler integration
        tau = 20.0  # ms
        C = 1.0  # nF
        
        # Leak term
        leak_term = (self.resting_potential - self.membrane_potential) / tau
        
        # Input term
        input_term = self.current_input / C
        
        # Update membrane potential
        self.membrane_potential += dt * 1000.0 * (leak_term + input_term)
        
        # Apply spike-frequency adaptation
        if self.adaptation_variable > 0:
            self.membrane_potential -= self.adaptation_variable * 0.01
            self.adaptation_variable *= 0.95
        
        # Clamp membrane potential
        self.membrane_potential = max(min(self.membrane_potential, 50.0), -100.0)
    
    def _handle_spike(self, current_time: float) -> bool:
        """Handle neuron spike."""
        self.firing_state = "active"
        self.last_spike_time = current_time
        self.spike_history.append(current_time)
        
        # Record spike in outgoing synapses
        for synapse in self.synapses_out:
            synapse.record_pre_spike(current_time)
        
        # Reset membrane potential
        self.membrane_potential = self.reset_potential
        
        # Enter refractory period
        self.refractory_remaining = self.refractory_period
        self.firing_state = "refractory"
        
        # Update adaptation variable
        self.adaptation_variable += 1.0
        
        return True
    
    def receive_excitatory_input(self, amplitude: float) -> None:
        """Receive excitatory synaptic input."""
        self.current_input += amplitude
    
    def receive_inhibitory_input(self, amplitude: float) -> None:
        """Receive inhibitory synaptic input."""
        self.current_input -= amplitude
    
    def inject_current(self, current: float) -> None:
        """Inject external current into neuron."""
        self.current_input += current
    
    def reset(self) -> None:
        """Reset neuron to initial state."""
        self.membrane_potential = self.resting_potential
        self.firing_state = "resting"
        self.refractory_remaining = 0
        self.firing_rate = 0.0
        self.spike_history.clear()
        self.current_input = 0.0
        self.adaptation_variable = 0.0
        self.last_spike_time = -1.0
    
    def initialize_random(self) -> None:
        """Initialize neuron with random biological parameters."""
        # Random membrane potential near resting
        self.membrane_potential = self.resting_potential + np.random.uniform(-3.0, 3.0)
        
        # Random threshold
        self.threshold = -55.0 + np.random.uniform(-2.0, 2.0)
        
        # Random resting potential
        self.resting_potential = -70.0 + np.random.uniform(-2.0, 2.0)
        
        # Random reset potential
        self.reset_potential = self.resting_potential + np.random.uniform(0.0, 5.0)
        
        # Random refractory period
        self.refractory_period = int(np.random.uniform(2, 10))
        
        # Initial state
        self.firing_state = "resting"
        self.refractory_remaining = 0
        self.adaptation_variable = 0.0
        self.last_spike_time = -1.0
    
    def get_state(self) -> Dict[str, Any]:
        """Get neuron state."""
        return {
            'id': self.id,
            'type': self.type,
            'membrane_potential': self.membrane_potential,
            'threshold': self.threshold,
            'resting_potential': self.resting_potential,
            'reset_potential': self.reset_potential,
            'refractory_period': self.refractory_period,
            'firing_rate': self.firing_rate,
            'firing_state': self.firing_state,
            'last_spike_time': self.last_spike_time,
            'spike_count': len(self.spike_history)
        }
    
    def from_state(state: Dict[str, Any]) -> 'LIFNeuron':
        """Create neuron from saved state."""
        neuron = LIFNeuron(state['id'], state['type'])
        neuron.__dict__.update({k: v for k, v in state.items() if k not in ['id', 'type']})
        return neuron
    
    def get_incoming_synapses(self) -> List[Synapse]:
        """Get incoming synapses."""
        return self.synapses_in
    
    def get_outgoing_synapses(self) -> List[Synapse]:
        """Get outgoing synapses."""
        return self.synapses_out
