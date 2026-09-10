# NLM - Python Neural Learning Machine API

import numpy as np
from typing import List, Optional, Dict, Any, Union
import json

class ConfigManager:
    """High-level configuration manager for NLM simulations."""
    
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


class Synapse:
    """Synaptic connection between neurons with advanced plasticity."""
    
    def __init__(self, source, destination, initial_weight: float = 0.0):
        self.source = source
        self.destination = destination
        self.weight = initial_weight
        self.initial_weight = initial_weight
        self.delay = 1
        self.type = "excitatory"
        self.plasticity_flags = {
            'hebbian': True,
            'stdp': True,
            'reward_modulated': False
        }
        self.short_term_facilitation = 0.0
        self.short_term_depression = 1.0
        self.last_pre_spike_time = -1.0
        self.last_post_spike_time = -1.0
        self.eligibility_trace = 0.0
        self.efficacy = 1.0
        self.release_probability = 0.5
        self.pre_spike_history = []
        self.post_spike_history = []
    
    def record_pre_spike(self, timestamp: float) -> None:
        self.pre_spike_history.append(timestamp)
        self.last_pre_spike_time = timestamp
        self.eligibility_trace += 1.0
    
    def record_post_spike(self, timestamp: float) -> None:
        self.post_spike_history.append(timestamp)
        self.last_post_spike_time = timestamp
    
    def get_weight(self) -> float:
        return self.weight
    
    def set_weight(self, weight: float) -> None:
        self.weight = max(-1.0, min(1.0, weight))
    
    def add_to_weight(self, delta: float) -> None:
        self.set_weight(self.weight + delta)
    
    def get_efficacy(self) -> float:
        return self.efficacy
    
    def set_efficacy(self, efficacy: float) -> None:
        self.efficacy = max(0.0, min(2.0, efficacy))
    
    def step(self) -> None:
        # Update short-term plasticity
        self._update_short_term_plasticity()
        
        # Apply plasticity rules
        self._apply_plasticity()
        
        # Decay eligibility trace
        self.decay_eligibility_trace(0.001)
    
    def _update_short_term_plasticity(self) -> None:
        current_time = len(self.pre_spike_history) + len(self.post_spike_history)
        
        if self.last_pre_spike_time >= 0:
            time_since_pre = current_time - self.last_pre_spike_time
            self.short_term_facilitation *= np.exp(-time_since_pre / 100.0)
        
        if self.last_post_spike_time >= 0:
            time_since_post = current_time - self.last_post_spike_time
            self.short_term_depression += (1.0 - self.short_term_depression) * (1.0 - np.exp(-time_since_post / 200.0))
        
        self.efficacy = min(2.0, self.efficacy * self.short_term_facilitation * self.short_term_depression)
    
    def _apply_plasticity(self) -> None:
        if not self.pre_spike_history or not self.post_spike_history:
            return
        
        recent_pre = self.pre_spike_history[-1]
        recent_post = self.post_spike_history[-1]
        
        if recent_post > recent_pre:
            delta = 0.01 * self.efficacy
            self.add_to_weight(delta)
        elif recent_post < recent_pre:
            delta = -0.01 * self.efficacy
            self.add_to_weight(delta)
        
        if self.plasticity_flags['hebbian']:
            if self.pre_spike_history and self.post_spike_history:
                hebbian_delta = 0.005
                self.add_to_weight(hebbian_delta)
        
        if self.plasticity_flags['reward_modulated'] and self.eligibility_trace > 0:
            reward_modulation = self.efficacy * self.eligibility_trace * 0.001
            self.add_to_weight(reward_modulation)
    
    def decay_eligibility_trace(self, decay_rate: float) -> None:
        self.eligibility_trace *= (1.0 - decay_rate)
        if abs(self.eligibility_trace) < 0.001:
            self.eligibility_trace = 0.0
    
    def reset(self) -> None:
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


class LIFNeuron:
    """Leaky Integrate-and-Fire neuron with advanced dynamics."""
    
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
        return self.firing_state == "active" or self.firing_state == "refractory"
    
    def add_incoming_synapse(self, synapse: Synapse) -> None:
        self.synapses_in.append(synapse)
    
    def add_outgoing_synapse(self, synapse: Synapse) -> None:
        self.synapses_out.append(synapse)
    
    def step(self, current_time: float, dt: float = 0.001) -> bool:
        if self.refractory_remaining > 0:
            self.refractory_remaining -= 1
            if self.refractory_remaining == 0:
                self.firing_state = "resting"
            return False
        
        self._update_membrane_potential(dt)
        
        if self.membrane_potential >= self.threshold:
            return self._handle_spike(current_time)
        else:
            self.firing_state = "active"
        
        self.current_input = 0.0
        return False
    
    def _update_membrane_potential(self, dt: float) -> None:
        tau = 20.0
        C = 1.0
        
        leak_term = (self.resting_potential - self.membrane_potential) / tau
        input_term = self.current_input / C
        
        self.membrane_potential += dt * 1000.0 * (leak_term + input_term)
        
        if self.adaptation_variable > 0:
            self.membrane_potential -= self.adaptation_variable * 0.01
            self.adaptation_variable *= 0.95
        
        self.membrane_potential = max(min(self.membrane_potential, 50.0), -100.0)
    
    def _handle_spike(self, current_time: float) -> bool:
        self.firing_state = "active"
        self.last_spike_time = current_time
        self.spike_history.append(current_time)
        
        for synapse in self.synapses_out:
            synapse.record_pre_spike(current_time)
        
        self.membrane_potential = self.reset_potential
        self.refractory_remaining = self.refractory_period
        self.firing_state = "refractory"
        
        self.adaptation_variable += 1.0
        return True
    
    def receive_excitatory_input(self, amplitude: float) -> None:
        self.current_input += amplitude
    
    def receive_inhibitory_input(self, amplitude: float) -> None:
        self.current_input -= amplitude
    
    def inject_current(self, current: float) -> None:
        self.current_input += current
    
    def reset(self) -> None:
        self.membrane_potential = self.resting_potential
        self.firing_state = "resting"
        self.refractory_remaining = 0
        self.firing_rate = 0.0
        self.spike_history.clear()
        self.current_input = 0.0
        self.adaptation_variable = 0.0
        self.last_spike_time = -1.0
    
    def initialize_random(self) -> None:
        self.membrane_potential = self.resting_potential + np.random.uniform(-3.0, 3.0)
        self.threshold = -55.0 + np.random.uniform(-2.0, 2.0)
        self.resting_potential = -70.0 + np.random.uniform(-2.0, 2.0)
        self.reset_potential = self.resting_potential + np.random.uniform(0.0, 5.0)
        self.refractory_period = int(np.random.uniform(2, 10))
        
        self.firing_state = "resting"
        self.refractory_remaining = 0
        self.adaptation_variable = 0.0
        self.last_spike_time = -1.0
    
    def get_state(self) -> Dict[str, Any]:
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


class NeuralNetwork:
    """High-level neural network interface with advanced features."""
    
    def __init__(self, config: Optional[ConfigManager] = None):
        if config is None:
            self._config = ConfigManager()
        else:
            self._config = config
        
        self._brain = self._initialize_brain()
        self._neurons = []
        self._synapses = []
        self._development_stage = "initial"
    
    def _initialize_brain(self) -> Dict[str, Any]:
        return {
            'neuron_count': self._config.get('brain.neuron_count', 1000),
            'synapse_density': self._config.get('brain.synapse_density', 0.1),
            'connection_probability': self._config.get('brain.connection_probability', 0.05),
            'simulation_timestep': self._config.get('simulation_timestep', 0.001),
            'plasticity': {
                'stdp': {'enable': self._config.get('plasticity.stdp.enable', True)},
                'hebbian': {'enable': self._config.get('plasticity.hebbian.enable', True)},
                'structural': {'enable': self._config.get('plasticity.structural.enable', True)}
            }
        }
    
    def initialize(self) -> None:
        self._reset_neurons()
        self._initialize_neurons_randomly()
        self._create_synaptic_connections()
    
    def _reset_neurons(self) -> None:
        for neuron in self._neurons:
            neuron.reset()
    
    def _initialize_neurons_randomly(self) -> None:
        for i in range(self._brain['neuron_count']):
            neuron = LIFNeuron(i, "internal")
            neuron.initialize_random()
            self._neurons.append(neuron)
    
    def _create_synaptic_connections(self) -> None:
        num_neurons = len(self._neurons)
        num_connections = int(num_neurons * num_neurons * self._brain['synapse_density'])
        
        for _ in range(num_connections):
            if np.random.random() < self._brain['connection_probability']:
                source = np.random.randint(0, num_neurons)
                dest = np.random.randint(0, num_neurons)
                if source != dest:
                    self._create_synapse(source, dest)
    
    def _create_synapse(self, source_idx: int, dest_idx: int) -> None:
        synapse = Synapse(self._neurons[source_idx], self._neurons[dest_idx])
        synapse.initialize_random()
        
        self._neurons[source_idx].add_outgoing_synapse(synapse)
        self._neurons[dest_idx].add_incoming_synapse(synapse)
        self._synapses.append(synapse)
    
    def step(self, step: int, time: float = None) -> None:
        dt = self._brain['simulation_timestep']
        current_time = time if time is not None else step * dt
        
        for neuron in self._neurons:
            neuron.step(current_time, dt)
        
        for synapse in self._synapses:
            synapse.step()
        
        self._update_development()
    
    def _update_development(self) -> None:
        total_steps = sum(len(neuron.spike_history) for neuron in self._neurons)
        if total_steps < 100:
            self._development_stage = "initial"
        elif total_steps < 1000:
            self._development_stage = "critical_period"
        elif total_steps < 5000:
            self._development_stage = "maturation"
        else:
            self._development_stage = "adult"
    
    def reset(self) -> None:
        self.initialize()
    
    def get_neuron_count(self) -> int:
        return len(self._neurons)
    
    def get_synapse_count(self) -> int:
        return len(self._synapses)
    
    def get_firing_neurons(self) -> int:
        return sum(1 for neuron in self._neurons if neuron.is_firing())
    
    def get_total_spikes(self) -> int:
        return sum(len(neuron.spike_history) for neuron in self._neurons)
    
    def get_average_firing_rate(self) -> float:
        if not self._neurons:
            return 0.0
        total_firing_rate = sum(neuron.firing_rate for neuron in self._neurons)
        return total_firing_rate / len(self._neurons)
    
    def get_developmental_stage(self) -> str:
        return self._development_stage
    
    def save(self, filepath: str) -> bool:
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
        try:
            with open(filepath, 'r') as f:
                state = json.load(f)
            
            self._brain = state['brain_params']
            self._neurons = []
            self._synapses = []
            
            for neuron_state in state['neurons']:
                neuron = LIFNeuron(neuron_state['id'], neuron_state['type'])
                neuron.__dict__.update({k: v for k, v in neuron_state.items() if k not in ['id', 'type']})
                self._neurons.append(neuron)
            
            for synapse_state in state['synapses']:
                synapse = Synapse(None, None)
                synapse.__dict__.update(synapse_state)
                self._synapses.append(synapse)
            
            self._development_stage = state['development_stage']
            
            return True
        except Exception:
            return False


class SimpleWorld:
    """Simple world environment for agent interaction."""
    
    def __init__(self, width: int = 20, height: int = 20):
        self.width = width
        self.height = height
        self.simulation_time = 0.0
        self.agent_x = width // 2
        self.agent_y = height // 2
        self.agent_orientation = 0.0
        self.objects = []
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        self.width = width
        self.height = height
    
    def reset(self) -> None:
        self.simulation_time = 0.0
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.agent_orientation = 0.0
        self.objects = []
    
    def set_agent_start(self, x: float, y: float) -> None:
        self.agent_x = x
        self.agent_y = y
    
    def update(self, timestep: float) -> None:
        self.simulation_time += timestep
    
    def apply_motor_command(self, action, current_time: float) -> None:
        if hasattr(action, 'type'):
            if action.type == 'move_forward':
                self.agent_x += 1.0
            elif action.type == 'move_backward':
                self.agent_x -= 1.0
            elif action.type == 'turn_left':
                self.agent_orientation += 0.1
            elif action.type == 'turn_right':
                self.agent_orientation -= 0.1
        
        self.agent_x = max(0.0, min(self.agent_x, self.width - 1))
        self.agent_y = max(0.0, min(self.agent_y, self.height - 1))
    
    def get_sensory_percept(self) -> Any:
        class SimpleVision:
            def __init__(self, width: int, height: int):
                self.width = width
                self.height = height
                self.channels = 3
                self.data = np.random.random(width * height * 3).tolist()
                self.timestamp = self.simulation_time
            
            def get_type(self) -> str:
                return "vision"
            
            def get_data(self) -> List[float]:
                return self.data
            
            def get_dimensions(self) -> tuple:
                return (self.width, self.height)
            
            def get_timestamp(self) -> float:
                return self.timestamp
        
        return SimpleVision(8, 8)
    
    def get_agent_body(self) -> Dict[str, float]:
        return {
            'x': self.agent_x,
            'y': self.agent_y,
            'orientation': self.agent_orientation,
            'velocity_x': 0.0,
            'velocity_y': 0.0,
            'angular_velocity': 0.0,
            'energy': 100.0,
            'health': 100.0,
            'age': self.simulation_time,
            'is_moving': False,
            'is_turning': False,
            'last_action_time': self.simulation_time
        }


class AgentBrain:
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain: NeuralNetwork):
        self.brain = brain
    
    def initialize(self, world: SimpleWorld) -> None:
        pass
    
    def get_sensory_input_size(self) -> int:
        return 8 * 8 * 3  # Vision input size
    
    def get_motor_output_size(self) -> int:
        return 8  # Action types
    
    def process_sensory_input(self, percept) -> None:
        # Inject sensory input into brain
        pass
    
    def decode_motor_command(self):
        class SimpleAction:
            def __init__(self):
                self.type = "move_forward"
                self.parameters = [1.0, 0.0]
        return SimpleAction()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        pass
    
    def update_development(self, timestep: float) -> None:
        self.brain._update_development()
    
    def get_developmental_stage(self) -> str:
        return self.brain.get_developmental_stage()
    
    def get_brain(self) -> NeuralNetwork:
        return self.brain
    
    def enable_reward_modulation(self, enable: bool) -> None:
        pass
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        pass
    
    def enable_development(self, enable: bool) -> None:
        pass
    
    def enable_curiosity(self, enable: bool) -> None:
        pass


def createBrain(config):
    """Create a brain with configuration."""
    return NeuralNetwork(config)


def createDefaultConfig():
    """Create a default configuration."""
    return ConfigManager()


def createSimpleWorld():
    """Create a simple world."""
    return SimpleWorld()


def createAgentBrain(brain):
    """Create an agent brain interface."""
    return AgentBrain(brain)


__version__ = "0.1.0"
EXAMPLES = {
    "simple_brain": """
# Create a simple brain
brain = createBrain(createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.get_total_spikes()} spikes")

print(f"Total neurons: {brain.get_neuron_count()}")
print(f"Total synapses: {brain.get_synapse_count()}")
""",

    "agent_world": """
# Create brain, world, and agent
brain = createBrain(createDefaultConfig())
world = createSimpleWorld()
agent = createAgentBrain(brain)

# Initialize
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
agent.initialize(world)

# Run episode
for step in range(100):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.simulation_time)

print("Simulation complete!")
"""
}
