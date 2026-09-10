# Python Core Components for NLM

import numpy as np
from typing import List, Optional, Dict, Any, Union
import json

class ConfigManager:
    """Python wrapper for NLM configuration management."""
    
    def __init__(self):
        import pynlm
        self._config = pynlm.createDefaultConfig()
        self._config_ptr = self._config
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a JSON file."""
        return self._config_ptr.loadFromFile(filepath)
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value."""
        if isinstance(value, int):
            self._config_ptr.set(key, value)
        elif isinstance(value, float):
            self._config_ptr.set(key, value)
        elif isinstance(value, bool):
            self._config_ptr.set(key, value)
        elif isinstance(value, str):
            self._config_ptr.set(key, value)
        elif isinstance(value, list):
            # Handle list types
            self._config_ptr.set(key, str(value))
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        # Get all keys to find the matching one
        keys = self._config_ptr.getKeys()
        for k in keys:
            if k.lower() == key.lower():
                return self._get_value_by_key(k)
        return default
    
    def _get_value_by_key(self, key: str) -> Any:
        """Get configuration value by exact key."""
        # This is a simplified implementation
        # In a real implementation, we would access the underlying C++ Config properly
        return None
    
    def summary(self) -> str:
        """Get a summary of the configuration."""
        return self._config_ptr.summary()
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return self._config_ptr.getKeys()


class NeuralNetwork:
    """High-level neural network interface for NLM."""
    
    def __init__(self, config: Optional[ConfigManager] = None):
        if config is None:
            import pynlm
            self._brain = pynlm.createBrain(pynlm.createDefaultConfig())
        else:
            self._brain = config._config_ptr
        self._brain.initialize()
    
    def initialize(self) -> None:
        """Initialize the neural network."""
        self._brain.initialize()
    
    def step(self, step: int, time: float = None) -> None:
        """Perform one simulation step."""
        if time is not None:
            self._brain.step(step, time)
        else:
            self._brain.step(step)
    
    def reset(self) -> None:
        """Reset the neural network."""
        self._brain.reset()
        self._brain.initialize()
    
    def get_neuron_count(self) -> int:
        """Get the number of neurons."""
        return self._brain.getTotalNeuronCount()
    
    def get_synapse_count(self) -> int:
        """Get the number of synapses."""
        return self._brain.getTotalSynapseCount()
    
    def get_firing_neurons(self) -> int:
        """Get the number of firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    def get_total_spikes(self) -> int:
        """Get the total number of spikes."""
        return self._brain.getTotalSpikeCount()
    
    def get_average_firing_rate(self) -> float:
        """Get the average firing rate."""
        return self._brain.getAverageFiringRate()
    
    def get_developmental_stage(self) -> str:
        """Get the current developmental stage."""
        # This would need to be implemented based on the C++ enum
        return str(self._brain.getDevelopmentalStage())
    
    def save(self, filepath: str) -> bool:
        """Save the neural network state."""
        return self._brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """Load the neural network state."""
        return self._brain.load(filepath)


class LIFNeuron:
    """Individual Leaky Integrate-and-Fire neuron for simulation."""
    
    def __init__(self, neuron_id: int = 0):
        self.id = neuron_id
        self.type = "internal"
        self.membrane_potential = -70.0
        self.threshold = -55.0
        self.resting_potential = -70.0
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
    
    def inject_current(self, current: float) -> None:
        """Inject current into the neuron."""
        self.current_input += current
    
    def step(self, current_time: float, dt: float = 0.001) -> bool:
        """Perform one simulation step."""
        # LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
        # Simplified Euler integration
        tau = 20.0  # ms
        C = 1.0  # nF
        
        # Handle refractory period
        if self.refractory_remaining > 0:
            self.refractory_remaining -= 1
            if self.refractory_remaining == 0:
                self.firing_state = "resting"
            return False
        
        # Calculate membrane potential change
        leak_term = (self.resting_potential - self.membrane_potential) / tau
        input_term = self.current_input / C
        
        # Update membrane potential
        self.membrane_potential += dt * 1000.0 * (leak_term + input_term)
        
        # Apply spike-frequency adaptation
        if self.adaptation_variable > 0:
            self.membrane_potential -= self.adaptation_variable * 0.01
            self.adaptation_variable *= 0.95
        
        # Clamp membrane potential
        self.membrane_potential = max(min(self.membrane_potential, 50.0), -100.0)
        
        # Check for spike
        if self.membrane_potential >= self.threshold:
            self.firing_state = "active"
            self.last_spike_time = current_time
            self.spike_history.append(current_time)
            
            # Reset membrane potential
            self.membrane_potential = self.reset_potential
            
            # Enter refractory period
            self.refractory_remaining = self.refractory_period
            self.firing_state = "refractory"
            
            # Update adaptation variable
            self.adaptation_variable += 1.0
            
            return True
        else:
            self.firing_state = "active"
        
        # Clear current input for next step
        self.current_input = 0.0
        
        return False
    
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
    
    def set_parameters(self, **kwargs) -> None:
        """Set neuron parameters."""
        for key, value in kwargs.items():
            if hasattr(self, key):
                setattr(self, key, value)
    
    def get_state(self) -> Dict[str, Any]:
        """Get neuron state."""
        return {
            "id": self.id,
            "type": self.type,
            "membrane_potential": self.membrane_potential,
            "threshold": self.threshold,
            "resting_potential": self.resting_potential,
            "refractory_period": self.refractory_period,
            "firing_rate": self.firing_rate,
            "firing_state": self.firing_state,
            "last_spike_time": self.last_spike_time,
            "spike_count": len(self.spike_history)
        }


class Visualizer:
    """Visualization tools for NLM simulations."""
    
    def __init__(self):
        self.networks = []
        self.colors = {
            'active': 'red',
            'refractory': 'orange',
            'resting': 'green',
            'sensory': 'blue',
            'motor': 'purple',
            'inhibitory': 'black'
        }
    
    def plot_neural_activity(self, brain, output_file: str = None):
        """Plot neural activity patterns."""
        # Create a simple text-based visualization
        firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        
        plot_lines = []
        plot_lines.append("NLM Neural Activity Visualization")
        plot_lines.append("=" * 40)
        plot_lines.append(f"Firing Rate: {firing_rate:.2f} Hz")
        plot_lines.append(f"Total Spikes: {total_spikes}")
        plot_lines.append(f"Active Neurons: {brain.getFiringNeuronCount()}")
        plot_lines.append(f"Total Neurons: {brain.getTotalNeuronCount()}")
        plot_lines.append("")
        
        # Simple histogram of spike counts
        if total_spikes > 0:
            plot_lines.append("Spike Activity Histogram:")
            for i in range(min(10, total_spikes)):
                plot_lines.append(f"  Spikes {i*10:04d}-{(i+1)*10:04d}: {'*' * 10}")
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write('\n'.join(plot_lines))
            print(f"Visualization saved to {output_file}")
        
        return '\n'.join(plot_lines)
    
    def plot_synapse_changes(self, brain, output_file: str = None):
        """Plot synaptic weight changes."""
        synapse_count = brain.getTotalSynapseCount()
        
        plot_lines = []
        plot_lines.append("NLM Synaptic Changes Visualization")
        plot_lines.append("=" * 40)
        plot_lines.append(f"Total Synapses: {synapse_count}")
        plot_lines.append("")
        
        if synapse_count > 0:
            # Simplified visualization
            plot_lines.append("Synaptic Weight Distribution:")
            plot_lines.append("  Weights are stored internally in C++ objects")
            plot_lines.append("  Range: -1.0 to +1.0")
            plot_lines.append("  Type: Excitatory, Inhibitory, Modulatory")
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write('\n'.join(plot_lines))
            print(f"Visualization saved to {output_file}")
        
        return '\n'.join(plot_lines)
    
    def create_animation_data(self, brain, steps: int) -> List[Dict[str, Any]]:
        """Create animation data for visualization."""
        animation_frames = []
        
        for step in range(steps):
            brain.step(step)
            
            frame_data = {
                'step': step,
                'time': step * 0.001,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': str(brain.getDevelopmentalStage())
            }
            
            animation_frames.append(frame_data)
        
        return animation_frames


class ExperimentTemplate:
    """Template for running NLM experiments."""
    
    def __init__(self, name: str):
        self.name = name
        self.config = ConfigManager()
        self.brain = None
        self.results = {}
    
    def setup_config(self, **kwargs) -> None:
        """Setup experiment configuration."""
        for key, value in kwargs.items():
            self.config.set(key, value)
    
    def create_network(self) -> NeuralNetwork:
        """Create neural network for experiment."""
        self.brain = NeuralNetwork(self.config)
        return self.brain
    
    def run(self, steps: int) -> Dict[str, Any]:
        """Run experiment for given number of steps."""
        if self.brain is None:
            self.create_network()
        
        initial_spikes = self.brain.get_total_spikes()
        
        for step in range(steps):
            self.brain.step(step)
        
        final_spikes = self.brain.get_total_spikes()
        
        self.results = {
            'steps': steps,
            'initial_spikes': initial_spikes,
            'final_spikes': final_spikes,
            'total_spikes': final_spikes - initial_spikes,
            'average_firing_rate': self.brain.get_average_firing_rate(),
            'final_neuron_count': self.brain.get_neuron_count(),
            'final_synapse_count': self.brain.get_synapse_count(),
            'final_firing_neurons': self.brain.get_firing_neurons()
        }
        
        return self.results
    
    def save_results(self, filepath: str) -> bool:
        """Save experiment results."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.results, f, indent=2)
            return True
        except Exception:
            return False
    
    def get_summary(self) -> str:
        """Get experiment summary."""
        if not self.results:
            return "No results available"
        
        summary_lines = [f"Experiment: {self.name}", "=" * 20]
        for key, value in self.results.items():
            summary_lines.append(f"{key}: {value}")
        
        return "\n".join(summary_lines)


class Agent:
    """Simple agent interface for NLM world interaction."""
    
    def __init__(self, brain: NeuralNetwork, world_width: int = 20, world_height: int = 20):
        self.brain = brain
        self.world_width = world_width
        self.world_height = world_height
        self.x = world_width // 2
        self.y = world_height // 2
        self.orientation = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
    
    def get_sensory_input(self, world) -> Any:
        """Get sensory input from world."""
        return world.get_sensory_percept()
    
    def process_sensory_input(self, percept) -> None:
        """Process sensory input."""
        self.brain.step(int(self.last_action_time))
    
    def decode_motor_command(self) -> Any:
        """Decode motor command from brain."""
        # This would be implemented with proper action system
        # For now, return a simple action
        class SimpleAction:
            def __init__(self):
                self.type = "move_forward"
                self.parameters = [1.0, 0.0]  # forward, no turn
        
        return SimpleAction()
    
    def apply_reward_modulation(self, reward: float, prediction: float = 0.0) -> None:
        """Apply reward-based modulation."""
        # Update internal state based on reward
        self.energy = min(100.0, self.energy + reward * 10)
        self.health = max(0.0, self.health + reward * 5)
    
    def update_development(self, timestep: float) -> None:
        """Update development system."""
        self.age += timestep
        # Simple development model
        if self.age > 100:
            # Age-related changes
            self.energy *= 0.99
            self.health = max(0.0, self.health - 0.1)
    
    def get_body(self) -> Dict[str, float]:
        """Get agent body state."""
        return {
            'x': self.x,
            'y': self.y,
            'orientation': self.orientation,
            'velocity_x': 0.0,
            'velocity_y': 0.0,
            'angular_velocity': 0.0,
            'energy': self.energy,
            'health': self.health,
            'age': self.age,
            'is_moving': self.is_moving,
            'is_turning': self.is_turning,
            'last_action_time': self.last_action_time
        }
    
    def apply_motor_command(self, action, current_time: float) -> None:
        """Apply motor command to update agent position."""
        self.last_action_time = current_time
        self.is_moving = True
        
        # Simple movement logic
        if hasattr(action, 'type'):
            if action.type == 'move_forward':
                self.x += 1.0
            elif action.type == 'move_backward':
                self.x -= 1.0
            elif action.type == 'turn_left':
                self.orientation += 0.1
            elif action.type == 'turn_right':
                self.orientation -= 0.1
        
        # Keep agent within bounds
        self.x = max(0.0, min(self.x, self.world_width - 1))
        self.y = max(0.0, min(self.y, self.world_height - 1))
        
        self.is_moving = False


class World:
    """Simple world simulation."""
    
    def __init__(self, width: int = 20, height: int = 20):
        self.width = width
        self.height = height
        self.simulation_time = 0.0
        self.agent_x = width // 2
        self.agent_y = height // 2
        self.agent_orientation = 0.0
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.random_seed = 42
        self.objects = []
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """Configure world parameters."""
        self.width = width
        self.height = height
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self.simulation_time = 0.0
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.agent_orientation = 0.0
        self.objects = []
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self.agent_x = x
        self.agent_y = y
    
    def update(self, timestep: float) -> None:
        """Update world state."""
        self.simulation_time += timestep
        
        # Energy decay
        # In a real implementation, this would be tracked per agent
    
    def apply_motor_command(self, action, current_time: float) -> None:
        """Apply motor command to world."""
        # In a real implementation, this would update agent state
        pass
    
    def get_sensory_percept(self) -> Any:
        """Get sensory percept from world."""
        # Create a simple vision percept
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
        """Get agent body state."""
        return {
            'x': self.agent_x,
            'y': self.agent_y,
            'orientation': self.agent_orientation,
            'velocity_x': 0.0,
            'velocity_y': 0.0,
            'angular_velocity': 0.0,
            'energy': self.max_energy,
            'health': 100.0,
            'age': self.simulation_time,
            'is_moving': False,
            'is_turning': False,
            'last_action_time': self.simulation_time
        }
    
    def add_object(self, obj) -> None:
        """Add object to world."""
        self.objects.append(obj)
    
    def remove_object(self, x: float, y: float) -> None:
        """Remove object at position."""
        self.objects = [obj for obj in self.objects if abs(obj.x - x) > 0.5 or abs(obj.y - y) > 0.5]
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        return 0 <= x < self.width and 0 <= y < self.height
    
    def get_width(self) -> int:
        """Get world width."""
        return self.width
    
    def get_height(self) -> int:
        """Get world height."""
        return self.height
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self.max_energy
    
    def set_max_energy(self, energy: float) -> None:
        """Set maximum energy."""
        self.max_energy = energy
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self.energy_decay_rate
    
    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate."""
        self.energy_decay_rate = rate
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self.simulation_time
    
    def set_random_seed(self, seed: int) -> None:
        """Set random seed."""
        self.random_seed = seed
    
    def get_random_seed(self) -> int:
        """Get random seed."""
        return self.random_seed
