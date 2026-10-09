"""
Core Python implementation for NLM (Neural Learning Machine)

This module provides pure Python implementations of NLM components
when C++ bindings are not available, along with enhanced functionality
and utilities.
"""

import numpy as np
from typing import List, Optional, Dict, Any, Union
from dataclasses import dataclass
from enum import Enum
import math
import random
from abc import ABC, abstractmethod

__version__ = "0.1.0"


class ConfigError(Exception):
    """Exception raised for configuration errors."""
    pass


class NeuralError(Exception):
    """Exception raised for neural network errors."""
    pass


class WorldError(Exception):
    """Exception raised for world simulation errors."""
    pass


class SimulationError(Exception):
    """Exception raised for simulation errors."""
    pass


@dataclass
class NeuronState:
    """State representation for a neuron."""
    id: int
    position: tuple
    potential: float
    threshold: float
    refractory_steps: int
    last_spike_time: float
    type: str = "excitatory"


@dataclass
class SynapseState:
    """State representation for a synapse."""
    id: int
    source_id: int
    target_id: int
    weight: float
    delay: int
    type: str = "excitatory"
    enabled: bool = True


@dataclass
class SimulationStep:
    """Represents a single simulation step."""
    step: int
    time: float
    spiking_neurons: List[int]
    active_synapses: List[int]
    total_spikes: int
    firing_rate: float
    reward: float = 0.0


class Config:
    """Configuration management for NLM."""
    
    def __init__(self):
        self._settings: Dict[str, Any] = {}
        self._default_settings = {
            'neuron_count': 1000,
            'region_count': 1,
            'connection_probability': 0.05,
            'initial_weight_mean': 0.5,
            'initial_weight_std': 0.1,
            'v_thresh': -50.0,
            'v_rest': -70.0,
            'v_reset': -75.0,
            'tau_mem': 20.0,
            'tau_ref': 2.0,
            'simulation_timestep': 0.001,
            'random_seed': 42,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_tau': 20.0,
            'synaptogenesis_rate': 0.0001,
            'pruning_rate': 0.00001,
        }
        
        # Load defaults
        for key, value in self._default_settings.items():
            self.set(key, value)
    
    def set(self, key: str, value: Any, description: str = "") -> None:
        """Set a configuration value."""
        self._settings[key] = {
            'value': value,
            'description': description,
            'source': 'runtime'
        }
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        if key in self._settings:
            return self._settings[key]['value']
        elif default is not None:
            return default
        elif key in self._default_settings:
            return self._default_settings[key]
        else:
            raise ConfigError(f"Unknown configuration key: {key}")
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key."""
        return key in self._settings or key in self._default_settings
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from file (JSON, YAML, or key=value format)."""
        try:
            # Auto-detect file format based on extension
            if filepath.endswith('.json'):
                return self._load_from_json(filepath)
            elif filepath.endswith(('.yaml', '.yml')):
                return self._load_from_yaml(filepath)
            else:
                # Default to key=value format
                return self._load_from_key_value(filepath)
        except Exception as e:
            raise ConfigError(f"Failed to load configuration from {filepath}: {e}")
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to file."""
        try:
            if filepath.endswith('.json'):
                return self._save_to_json(filepath)
            elif filepath.endswith(('.yaml', '.yml')):
                return self._save_to_yaml(filepath)
            else:
                return self._save_to_key_value(filepath)
        except Exception as e:
            raise ConfigError(f"Failed to save configuration to {filepath}: {e}")
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._settings.keys()) + list(self._default_settings.keys())
    
    def clear(self) -> None:
        """Clear all settings."""
        self._settings.clear()
    
    def remove(self, key: str) -> None:
        """Remove a configuration key."""
        if key in self._settings:
            del self._settings[key]
    
    def get_or(self, key: str, default: Any) -> Any:
        """Get configuration value or default if not set."""
        try:
            return self.get(key)
        except ConfigError:
            return default
    
    def summary(self) -> str:
        """Get configuration summary."""
        lines = ["Configuration:"]
        for key in sorted(self.get_keys()):
            if key in self._settings:
                value = self._settings[key]['value']
                desc = self._settings[key]['description']
                source = self._settings[key]['source']
                lines.append(f"  {key} = {value} (source: {source})")
                if desc:
                    lines.append(f"    {desc}")
            else:
                value = self._default_settings[key]
                lines.append(f"  {key} = {value} (default)")
        return "\n".join(lines)
    
    def _load_from_key_value(self, filepath: str) -> bool:
        """Load configuration from key=value format."""
        try:
            with open(filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if not line or line.startswith('#') or line.startswith('/'):
                        continue
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Parse different types
                        if value.lower() == 'true':
                            parsed_value = True
                        elif value.lower() == 'false':
                            parsed_value = False
                        elif value.isdigit():
                            parsed_value = int(value)
                        elif value.replace('.', '').replace('-', '').isdigit():
                            parsed_value = float(value)
                        else:
                            # Remove quotes
                            if (value.startswith('"') and value.endswith('"')) or (
                                value.startswith("'") and value.endswith("'")
                            ):
                                parsed_value = value[1:-1]
                            else:
                                parsed_value = value
                        
                        self.set(key, parsed_value)
            return True
        except Exception as e:
            raise ConfigError(f"Failed to parse key=value file: {e}")
    
    def _save_to_key_value(self, filepath: str) -> bool:
        """Save configuration to key=value format."""
        try:
            with open(filepath, 'w') as f:
                f.write("# NLM Configuration File\n")
                f.write("# Format: key = value\n\n")
                
                for key in sorted(self.get_keys()):
                    if key in self._settings:
                        value = self._settings[key]['value']
                        desc = self._settings[key]['description']
                        if desc:
                            f.write(f"# {desc}\n")
                        f.write(f"{key} = {self._format_value(value)}\n\n")
                    else:
                        value = self._default_settings[key]
                        f.write(f"{key} = {self._format_value(value)}\n\n")
            return True
        except Exception as e:
            raise ConfigError(f"Failed to write key=value file: {e}")
    
    def _load_from_json(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
                self._apply_json_config(data)
            return True
        except Exception as e:
            raise ConfigError(f"Failed to parse JSON file: {e}")
    
    def _save_to_json(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
        try:
            config_data = self._to_json()
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            return True
        except Exception as e:
            raise ConfigError(f"Failed to write JSON file: {e}")
    
    def _apply_json_config(self, data: Dict[str, Any]) -> None:
        """Apply JSON configuration data."""
        for key, value in data.items():
            if isinstance(value, dict) and 'value' in value:
                # Full entry with description and source
                self._settings[key] = value
            else:
                # Simple value
                self.set(key, value)
    
    def _to_json(self) -> Dict[str, Any]:
        """Convert configuration to JSON format."""
        config_data = {}
        
        # Add custom settings
        for key, entry in self._settings.items():
            config_data[key] = {
                'value': entry['value'],
                'description': entry['description'],
                'source': entry['source']
            }
        
        # Add default settings
        for key, value in self._default_settings.items():
            if key not in config_data:
                config_data[key] = {
                    'value': value,
                    'description': f"Default value for {key}",
                    'source': 'default'
                }
        
        return config_data
    
    def _format_value(self, value: Any) -> str:
        """Format configuration value as string."""
        if isinstance(value, bool):
            return 'true' if value else 'false'
        elif isinstance(value, (int, float)):
            return str(value)
        elif isinstance(value, str):
            return f'"{value}"'
        else:
            return str(value)


class SimpleNeuron:
    """Simple neuron implementation for Python-only mode."""
    
    def __init__(self, neuron_id: int, neuron_type: str = "excitatory",
                 position: tuple = (0.0, 0.0), config: Optional[Config] = None):
        self.id = neuron_id
        self.type = neuron_type
        self.position = position
        self.config = config or Config()
        
        # Neuron parameters
        self.v = self.config.get('v_rest')
        self.v_thresh = self.config.get('v_thresh')
        self.v_reset = self.config.get('v_reset')
        self.tau_mem = self.config.get('tau_mem')
        self.tau_ref = self.config.get('tau_ref')
        
        # State
        self.refractory_steps = 0
        self.last_spike_time = 0.0
        self.connections: List[Dict] = []
        
        # Internal state
        self.input_current = 0.0
        self.time_constant = 0.0
        
    def inject_current(self, current: float) -> None:
        """Inject current into neuron."""
        if self.refractory_steps <= 0:
            self.input_current += current
    
    def update(self, dt: float, current_time: float) -> bool:
        """Update neuron state. Returns True if neuron spiked."""
        if self.refractory_steps > 0:
            self.refractory_steps -= 1
            if self.refractory_steps == 0:
                self.v = self.v_reset
            return False
        
        # Update membrane potential
        self.v += self.input_current * dt / self.tau_mem
        
        # Check for spike
        spiked = self.v >= self.v_thresh
        
        if spiked:
            self.v = self.v_reset
            self.refractory_steps = int(self.tau_ref / self.config.get('simulation_timestep'))
            self.last_spike_time = current_time
        
        # Decay current
        self.input_current *= 0.95  # Simple decay
        
        return spiked
    
    def get_potential(self) -> float:
        """Get membrane potential."""
        return self.v
    
    def get_threshold(self) -> float:
        """Get threshold potential."""
        return self.v_thresh
    
    def is_spiking(self) -> bool:
        """Check if neuron is currently spiking."""
        return self.refractory_steps > 0


class SimpleSynapse:
    """Simple synapse implementation for Python-only mode."""
    
    def __init__(self, syn_id: int, source_id: int, target_id: int,
                 synapse_type: str = "excitatory", config: Optional[Config] = None):
        self.id = syn_id
        self.source_id = source_id
        self.target_id = target_id
        self.type = synapse_type
        self.config = config or Config()
        
        # Synapse parameters
        self.weight = self._initialize_weight()
        self.delay = self._initialize_delay()
        self.enabled = True
        
        # Spike history for STDP
        self.pre_spike_times: List[float] = []
        self.post_spike_times: List[float] = []
        
        # Learning traces
        self.eligibility_trace = 0.0
        self.pre_trace = 0.0
        self.post_trace = 0.0
    
    def _initialize_weight(self) -> float:
        """Initialize random weight."""
        import random
        base_weight = self.config.get('initial_weight_mean')
        std_weight = self.config.get('initial_weight_std')
        return random.gauss(base_weight, std_weight)
    
    def _initialize_delay(self) -> int:
        """Initialize random delay."""
        return random.randint(1, 5)  # 1-5 ms delay
    
    def update(self, dt: float, current_time: float) -> None:
        """Update synapse traces."""
        # Decay traces
        self.pre_trace *= 0.95
        self.post_trace *= 0.95
        self.eligibility_trace *= 0.95
        
        # Clean old spike history
        self.pre_spike_times = [t for t in self.pre_spike_times if current_time - t < 100.0]
        self.post_spike_times = [t for t in self.post_spike_times if current_time - t < 100.0]
    
    def record_pre_spike(self, time: float) -> None:
        """Record pre-synaptic spike."""
        self.pre_spike_times.append(time)
        self.pre_trace += 1.0
        
        # Update eligibility trace for STDP
        if self.enabled:
            self.eligibility_trace += 1.0
    
    def record_post_spike(self, time: float) -> None:
        """Record post-synaptic spike."""
        self.post_spike_times.append(time)
        self.post_trace += 1.0
        
        # Update eligibility trace for STDP
        if self.enabled:
            self.eligibility_trace += 1.0
    
    def apply_stdp(self) -> None:
        """Apply STDP learning rule."""
        if not self.enabled:
            return
        
        # Calculate STDP weight change
        weight_change = 0.0
        
        # Look for recent pre-post spike pairs (LTP)
        for post_time in self.post_spike_times:
            for pre_time in self.pre_spike_times:
                if post_time > pre_time and post_time - pre_time < 50.0:  # 50ms window
                    weight_change += self.config.get('stdp_ltp_weight')
        
        # Look for recent post-pre spike pairs (LTD)
        for pre_time in self.pre_spike_times:
            for post_time in self.post_spike_times:
                if pre_time > post_time and pre_time - post_time < 50.0:  # 50ms window
                    weight_change -= self.config.get('stdp_ltd_weight')
        
        # Apply weight change with bounds
        self.weight += weight_change
        self.weight = max(-1.0, min(1.0, self.weight))  # Clamp to [-1, 1]
    
    def get_weight(self) -> float:
        """Get synapse weight."""
        return self.weight
    
    def set_weight(self, weight: float) -> None:
        """Set synapse weight."""
        self.weight = max(-1.0, min(1.0, weight))
    
    def add_to_weight(self, delta: float) -> None:
        """Add to synapse weight."""
        self.set_weight(self.weight + delta)
    
    def get_delay(self) -> int:
        """Get synapse delay."""
        return self.delay
    
    def set_delay(self, delay: int) -> None:
        """Set synapse delay."""
        self.delay = max(1, delay)
    
    def get_type(self) -> str:
        """Get synapse type."""
        return self.type
    
    def is_excitatory(self) -> bool:
        """Check if synapse is excitatory."""
        return self.type == "excitatory"
    
    def is_inhibitory(self) -> bool:
        """Check if synapse is inhibitory."""
        return self.type == "inhibitory"
    
    def reset(self) -> None:
        """Reset synapse to initial state."""
        self.weight = self._initialize_weight()
        self.delay = self._initialize_delay()
        self.enabled = True
        self.pre_spike_times.clear()
        self.post_spike_times.clear()
        self.eligibility_trace = 0.0
        self.pre_trace = 0.0
        self.post_trace = 0.0


class Brain:
    """Simple brain implementation for Python-only mode."""
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config or Config()
        
        # Brain components
        self.neurons: Dict[int, SimpleNeuron] = {}
        self.synapses: Dict[int, SimpleSynapse] = {}
        self.regions: Dict[int, List[int]] = {}
        
        # Statistics
        self.total_spikes = 0
        self.firing_count = 0
        self.current_step = 0
        
        # Initialize brain
        self._initialize_brain()
    
    def _initialize_brain(self) -> None:
        """Initialize brain components."""
        neuron_count = self.config.get('neuron_count')
        region_count = self.config.get('region_count')
        
        # Create neurons
        for i in range(neuron_count):
            neuron_type = "excitatory" if i % 3 != 0 else "inhibitory"
            self.neurons[i] = SimpleNeuron(i, neuron_type, config=self.config)
        
        # Create regions
        neurons_per_region = neuron_count // region_count
        for region_id in range(region_count):
            start_idx = region_id * neurons_per_region
            end_idx = min(start_idx + neurons_per_region, neuron_count)
            self.regions[region_id] = list(range(start_idx, end_idx))
        
        # Create synapses
        self._create_synapses()
    
    def _create_synapses(self) -> None:
        """Create random synapses between neurons."""
        syn_id = 0
        connection_prob = self.config.get('connection_probability')
        
        for source_id in self.neurons:
            for target_id in self.neurons:
                if source_id != target_id:
                    if random.random() < connection_prob:
                        synapse_type = "excitatory" if random.random() < 0.8 else "inhibitory"
                        self.synapses[syn_id] = SimpleSynapse(
                            syn_id, source_id, target_id, synapse_type, self.config
                        )
                        syn_id += 1
    
    def initialize(self) -> None:
        """Initialize brain."""
        # Reset all neurons
        for neuron in self.neurons.values():
            neuron.refractory_steps = 0
            neuron.v = self.config.get('v_reset')
        
        # Reset all synapses
        for synapse in self.synapses.values():
            synapse.reset()
        
        self.total_spikes = 0
        self.firing_count = 0
        self.current_step = 0
    
    def step(self, step_number: int, time: Optional[float] = None) -> None:
        """Advance brain simulation by one step."""
        if time is None:
            time = step_number * self.config.get('simulation_timestep')
        
        self.current_step = step_number
        
        # Update all neurons
        spiked_neurons = []
        for neuron in self.neurons.values():
            if neuron.update(self.config.get('simulation_timestep'), time):
                spiked_neurons.append(neuron.id)
                self.total_spikes += 1
        
        # Record spikes for synapses
        for synapse in self.synapses.values():
            if synapse.target_id in spiked_neurons:
                synapse.record_post_spike(time)
        
        # Update synapses
        for synapse in self.synapses.values():
            # Record pre-synaptic spikes
            if synapse.source_id in spiked_neurons:
                synapse.record_pre_spike(time)
            
            # Apply STDP
            synapse.apply_stdp()
            
            # Update traces
            synapse.update(self.config.get('simulation_timestep'), time)
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons."""
        return len(self.neurons)
    
    def get_total_synapse_count(self) -> int:
        """Get total number of synapses."""
        return len(self.synapses)
    
    def get_firing_neuron_count(self) -> int:
        """Get number of neurons currently firing."""
        return sum(1 for neuron in self.neurons.values() if neuron.is_spiking())
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        if self.current_step == 0:
            return 0.0
        return self.total_spikes / self.current_step
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self.total_spikes
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get E/I balance ratio."""
        exc_synapses = sum(1 for syn in self.synapses.values() if syn.is_excitatory())
        inh_synapses = sum(1 for syn in self.synapses.values() if syn.is_inhibitory())
        
        if exc_synapses + inh_synapses == 0:
            return 1.0
        
        return exc_synapses / (exc_synapses + inh_synapses)
    
    def get_developmental_stage(self) -> str:
        """Get developmental stage based on spike activity."""
        firing_rate = self.get_average_firing_rate()
        
        if firing_rate < 0.1:
            return "Initial"
        elif firing_rate < 1.0:
            return "Critical Period"
        elif firing_rate < 5.0:
            return "Maturation"
        elif firing_rate < 20.0:
            return "Adult"
        else:
            return "Aging"
    
    def add_region(self, name: str) -> int:
        """Add a new region to the brain."""
        region_id = len(self.regions)
        self.regions[region_id] = []
        return region_id
    
    def get_region(self, region_id: int) -> Optional[List[int]]:
        """Get neurons in a region."""
        return self.regions.get(region_id)
    
    def inject_current_to_neurons(self, neuron_type: str, current: float) -> None:
        """Inject current into neurons of a specific type."""
        count = 0
        for neuron in self.neurons.values():
            if neuron.type == neuron_type and count < 10:
                neuron.inject_current(current)
                count += 1
    
    def save(self, filename: str) -> None:
        """Save brain state to file."""
        data = {
            'neurons': {},
            'synapses': {},
            'regions': self.regions,
            'total_spikes': self.total_spikes,
            'firing_count': self.firing_count,
            'current_step': self.current_step,
            'version': __version__
        }
        
        for neuron_id, neuron in self.neurons.items():
            data['neurons'][str(neuron_id)] = {
                'id': neuron.id,
                'position': neuron.position,
                'potential': neuron.get_potential(),
                'threshold': neuron.get_threshold(),
                'refractory_steps': neuron.refractory_steps,
                'last_spike_time': neuron.last_spike_time,
                'type': neuron.type
            }
        
        for syn_id, synapse in self.synapses.items():
            data['synapses'][str(syn_id)] = {
                'id': synapse.id,
                'source_id': synapse.source_id,
                'target_id': synapse.target_id,
                'weight': synapse.get_weight(),
                'delay': synapse.get_delay(),
                'type': synapse.get_type(),
                'enabled': synapse.enabled,
                'pre_spike_times': synapse.pre_spike_times,
                'post_spike_times': synapse.post_spike_times,
                'eligibility_trace': synapse.eligibility_trace
            }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
    
    def load(self, filename: str) -> None:
        """Load brain state from file."""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        self.neurons.clear()
        self.synapses.clear()
        
        # Load neurons
        for neuron_id_str, neuron_data in data['neurons'].items():
            neuron = SimpleNeuron(
                neuron_data['id'],
                neuron_data['type'],
                neuron_data['position'],
                self.config
            )
            neuron.refractory_steps = neuron_data['refractory_steps']
            neuron.last_spike_time = neuron_data['last_spike_time']
            # Note: Cannot restore membrane potential directly
            self.neurons[neuron_data['id']] = neuron
        
        # Load synapses
        for syn_id_str, synapse_data in data['synapses'].items():
            synapse = SimpleSynapse(
                synapse_data['id'],
                synapse_data['source_id'],
                synapse_data['target_id'],
                synapse_data['type'],
                self.config
            )
            synapse.set_weight(synapse_data['weight'])
            synapse.set_delay(synapse_data['delay'])
            synapse.enabled = synapse_data['enabled']
            synapse.pre_spike_times = synapse_data['pre_spike_times']
            synapse.post_spike_times = synapse_data['post_spike_times']
            synapse.eligibility_trace = synapse_data['eligibility_trace']
            self.synapses[synapse_data['id']] = synapse
        
        # Restore brain state
        self.regions = data['regions']
        self.total_spikes = data['total_spikes']
        self.firing_count = data['firing_count']
        self.current_step = data['current_step']
    
    def log_status(self) -> None:
        """Log brain status."""
        print(f"Brain Status:")
        print(f"  Neurons: {self.get_total_neuron_count()}")
        print(f"  Synapses: {self.get_total_synapse_count()}")
        print(f"  Firing neurons: {self.get_firing_neuron_count()}")
        print(f"  Total spikes: {self.total_spikes}")
        print(f"  Average firing rate: {self.get_average_firing_rate():.2f}")
        print(f"  Developmental stage: {self.get_developmental_stage()}")
        print(f"  Current step: {self.current_step}")


# Monkey-patch to add additional methods to Brain for compatibility
Brain.enable_reward_modulation = lambda self, enable: None
Brain.enable_structural_plasticity = lambda self, enable: None
Brain.enable_development = lambda self, enable: None
Brain.enable_curiosity = lambda self, enable: None
Brain.get_curiosity_level = lambda self: 0.0
Brain.get_novelty_level = lambda self: 0.0
Brain.get_prediction_error = lambda self: 0.0
Brain.get_neuromodulation_level = lambda self: 0.0
Brain.apply_reward_modulation = lambda self, reward, predicted: None
Brain.update_development = lambda self, timestep: None


# Enums
class NeuronType:
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    SENSORY = "sensory"
    MOTOR = "motor"
    MODULATORY = "modulatory"
    INTERNAL = "internal"

    @classmethod
    def get_names(cls) -> Dict[str, str]:
        return {
            'excitatory': cls.EXCITATORY,
            'inhibitory': cls.INHIBITORY,
            'sensory': cls.SENSORY,
            'motor': cls.MOTOR,
            'modulatory': cls.MODULATORY,
            'internal': cls.INTERNAL
        }


class DevelopmentalStage:
    INITIAL = "Initial"
    CRITICAL_PERIOD = "Critical Period"
    MATURATION = "Maturation"
    ADULT = "Adult"
    AGING = "Aging"

    @classmethod
    def get_names(cls) -> Dict[str, str]:
        return {
            'initial': cls.INITIAL,
            'critical_period': cls.CRITICAL_PERIOD,
            'maturation': cls.MATURATION,
            'adult': cls.ADULT,
            'aging': cls.AGING
        }


class ActionType:
    MOVE_FORWARD = "move_forward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    INTERACT = "interact"
    EAT = "eat"
    # ... more action types as needed

    @classmethod
    def get_names(cls) -> Dict[str, str]:
        return {
            'move_forward': cls.MOVE_FORWARD,
            'turn_left': cls.TURN_LEFT,
            'turn_right': cls.TURN_RIGHT,
            'interact': cls.INTERACT,
            'eat': cls.EAT
        }


class WorldObjectType:
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

    @classmethod
    def get_names(cls) -> Dict[str, str]:
        return {
            'empty': cls.EMPTY,
            'resource': cls.RESOURCE,
            'hazard': cls.HAZARD,
            'wall': cls.WALL,
            'marker': cls.MARKER
        }


# Python API factory functions
def create_default_config() -> Config:
    """Create a default configuration for NLM."""
    return Config()


def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a neural brain for NLM.
    
    Args:
        config: Optional configuration. If None, uses default config.
        
    Returns:
        A Brain object ready for simulation.
    """
    return Brain(config)


# Monkey-patch to add compatibility with C++ API
def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface."""
    return AgentBrain(brain)


def create_simple_world() -> 'SimpleWorld':
    """Create a simple 2D world for NLM simulations."""
    return SimpleWorld()


# Simple world implementation for Python-only mode
class SimpleWorld:
    """Simple world implementation for Python-only mode."""
    
    def __init__(self):
        self.width = 20
        self.height = 20
        self.vision_width = 8
        self.vision_height = 8
        self.vision_angle = 0.0
        self.sim_time = 0.0
        
        # Agent position and state
        self.agent_x = 10.0
        self.agent_y = 10.0
        self.agent_angle = 0.0
        
        # World objects
        self.objects: Dict[tuple, Dict] = {}
        
        # Reset world
        self.reset()
    
    def configure(self, width: int, height: int, 
                  vision_width: int = 8, vision_height: int = 8,
                  vision_angle: float = 0.0) -> None:
        """Configure world dimensions."""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.vision_angle = vision_angle
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self.sim_time = 0.0
        self.agent_x = self.width / 2.0
        self.agent_y = self.height / 2.0
        self.agent_angle = 0.0
        self.objects.clear()
    
    def update(self, timestep: float) -> None:
        """Update world state."""
        self.sim_time += timestep
        
        # Update agent position based on actions
        # This is a simplified implementation
        pass
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self.agent_x = x
        self.agent_y = y
    
    def apply_motor_command(self, action: Dict, time: float) -> None:
        """Apply motor command to world."""
        action_type = action.get('type', ActionType.MOVE_FORWARD)
        
        if action_type == ActionType.MOVE_FORWARD:
            angle_rad = math.radians(self.agent_angle)
            self.agent_x += math.cos(angle_rad) * 0.5
            self.agent_y += math.sin(angle_rad) * 0.5
        elif action_type == ActionType.TURN_LEFT:
            self.agent_angle += 15.0
        elif action_type == ActionType.TURN_RIGHT:
            self.agent_angle -= 15.0
        elif action_type == ActionType.INTERACT:
            # Interaction logic
            pass
        
        # Keep agent in bounds
        self.agent_x = max(0.0, min(self.width, self.agent_x))
        self.agent_y = max(0.0, min(self.height, self.agent_y))
    
    def get_sensory_percept(self) -> Dict[str, Any]:
        """Get sensory input from world."""
        # Create a simple percept
        percept = {
            'vision': [],
            'position': {'x': self.agent_x, 'y': self.agent_y},
            'angle': self.agent_angle,
            'time': self.sim_time,
            'internal': [self.get_simulation_time() % 100.0]  # Simple internal signal
        }
        
        # Add vision data
        for dx in range(-self.vision_width // 2, self.vision_width // 2 + 1):
            for dy in range(-self.vision_height // 2, self.vision_height // 2 + 1):
                x = int(self.agent_x + dx * 2.0)
                y = int(self.agent_y + dy * 2.0)
                
                if 0 <= x < self.width and 0 <= y < self.height:
                    obj = self.objects.get((x, y))
                    if obj:
                        percept['vision'].append({
                            'x': x, 'y': y,
                            'type': obj['type'],
                            'value': obj.get('value', 1.0)
                        })
        
        return percept
    
    def get_agent_body(self) -> Dict[str, Any]:
        """Get agent body state."""
        return {
            'position': {'x': self.agent_x, 'y': self.agent_y},
            'angle': self.agent_angle,
            'width': 1.0,
            'height': 1.0
        }
    
    def get_width(self) -> int:
        """Get world width."""
        return self.width
    
    def get_height(self) -> int:
        """Get world height."""
        return self.height
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self.sim_time
    
    def set_random_seed(self, seed: int) -> None:
        """Set random seed for world."""
        random.seed(seed)
    
    def add_object(self, x: float, y: float, obj_type: str, 
                  value: float = 1.0, radius: float = 1.0) -> None:
        """Add object to world."""
        self.objects[(int(x), int(y))] = {
            'type': obj_type,
            'value': value,
            'radius': radius
        }
    
    def remove_object(self, x: float, y: float) -> bool:
        """Remove object from world."""
        return self.objects.pop((int(x), int(y)), None) is not None
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        return (0.0 <= x <= self.width and 
                0.0 <= y <= self.height and
                not self.is_wall_position(x, y))
    
    def is_wall_position(self, x: float, y: float) -> bool:
        """Check if position is a wall."""
        return (int(x) % 5 == 0 and int(y) % 5 == 0)  # Simple wall pattern
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return 100.0
    
    def set_max_energy(self, max_energy: float) -> None:
        """Set maximum energy."""
        pass


# Agent brain implementation for Python-only mode
class AgentBrain:
    """Agent brain implementation for Python-only mode."""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.world = None
        
        # Neuromodulation flags
        self.reward_modulation_enabled = False
        self.structural_plasticity_enabled = False
        self.development_enabled = False
        self.curiosity_enabled = False
        
        # State
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        self.prediction_error = 0.0
        self.neuromodulation_level = 0.0
        self.development_stage = DevelopmentalStage.INITIAL
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize agent with world."""
        self.world = world
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        self.prediction_error = 0.0
        self.neuromodulation_level = 0.0
    
    def process_sensory_input(self, percept: Dict[str, Any]) -> None:
        """Process sensory input."""
        # Update novelty detection
        if percept:
            self.novelty_level = min(1.0, self.novelty_level + 0.1)
            
            # Detect curiosity-relevant patterns
            if self.curiosity_enabled:
                self.curiosity_level += len(percept.get('vision', [])) * 0.05
    
    def decode_motor_command(self) -> Dict[str, Any]:
        """Decode motor command from brain state."""
        # Get brain stats
        firing_rate = self.brain.get_average_firing_rate()
        neuron_count = self.brain.get_firing_neuron_count()
        
        # Simple decision making based on brain state
        if neuron_count == 0:
            action_type = ActionType.TURN_LEFT
        elif firing_rate < 0.5:
            action_type = ActionType.MOVE_FORWARD
        elif firing_rate < 2.0:
            action_type = ActionType.INTERACT
        else:
            action_type = ActionType.EAT
        
        return {'type': action_type, 'confidence': 1.0}
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward modulation."""
        if self.reward_modulation_enabled:
            self.prediction_error = abs(reward - predicted_reward)
            self.neuromodulation_level = min(1.0, self.neuromodulation_level + reward * 0.1)
    
    def update_development(self, timestep: float) -> None:
        """Update development."""
        if self.development_enabled:
            firing_rate = self.brain.get_average_firing_rate()
            
            # Update developmental stage
            if firing_rate < 0.1:
                self.development_stage = DevelopmentalStage.INITIAL
            elif firing_rate < 1.0:
                self.development_stage = DevelopmentalStage.CRITICAL_PERIOD
            elif firing_rate < 5.0:
                self.development_stage = DevelopmentalStage.MATURATION
            elif firing_rate < 20.0:
                self.development_stage = DevelopmentalStage.ADULT
            else:
                self.development_stage = DevelopmentalStage.AGING
    
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable/disable reward modulation."""
        self.reward_modulation_enabled = enable
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity."""
        self.structural_plasticity_enabled = enable
    
    def enable_development(self, enable: bool) -> None:
        """Enable/disable development."""
        self.development_enabled = enable
    
    def enable_curiosity(self, enable: bool) -> None:
        """Enable/disable curiosity."""
        self.curiosity_enabled = enable
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self.curiosity_level
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self.novelty_level
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self.prediction_error
    
    def get_neuromodulation_level(self) -> float:
        """Get neuromodulation level."""
        return self.neuromodulation_level


# Add compatibility methods to Brain
Brain.enable_reward_modulation = lambda self, enable: None
Brain.enable_structural_plasticity = lambda self, enable: None
Brain.enable_development = lambda self, enable: None
Brain.enable_curiosity = lambda self, enable: None
Brain.get_curiosity_level = lambda self: 0.0
Brain.get_novelty_level = lambda self: 0.0
Brain.get_prediction_error = lambda self: 0.0
Brain.get_neuromodulation_level = lambda self: 0.0
Brain.apply_reward_modulation = lambda self, reward, predicted: None
Brain.update_development = lambda self, timestep: None
Brain.get_developmental_stage = lambda self: DevelopmentalStage.INITIAL


# Python API factory functions
def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface."""
    return AgentBrain(brain)


def create_simple_world() -> SimpleWorld:
    """Create a simple 2D world for NLM simulations."""
    return SimpleWorld()


# Export main classes and functions
__all__ = [
    # Configuration
    'Config',
    'ConfigError',
    
    # Core components
    'Brain',
    'SimpleNeuron',
    'SimpleSynapse',
    
    # Agent components
    'AgentBrain',
    'SimpleWorld',
    
    # Enums
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'WorldObjectType',
    
    # Errors
    'NeuralError',
    'WorldError',
    'SimulationError',
    
    # State classes
    'NeuronState',
    'SynapseState',
    'SimulationStep',
    
    # Factory functions
    'create_default_config',
    'create_brain',
    'create_agent_brain',
    'create_simple_world',
    
    # Version
    '__version__'
]
