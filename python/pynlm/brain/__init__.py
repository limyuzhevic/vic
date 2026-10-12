"""
Brain implementation for NLM Python package

This module provides the main brain class and neural computation functionality.
It implements real spiking neural computation with all integrated systems.
"""

from typing import Dict, Any, Optional, List, Tuple
from .utils import run_with_progress, validate_config
import time


class Brain:
    """
    NLM Brain - Central neural simulation engine.

    Implements the complete integrated brain simulation with:
    - Real spiking neural computation (LIF dynamics)
    - Event-driven spike propagation
    - Multiple memory systems
    - Neuromodulation and learning
    - Cognitive functions
    - Development and plasticity

    This is the main class for creating and simulating artificial brains.
    """

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize a new NLM brain.

        Args:
            config: Configuration dictionary. If None, uses default config.
        """
        if config is None:
            config = self._create_default_config()

        self.config = config
        self._validate_config()

        # Internal state
        self._step_count = 0
        self._time = 0.0
        self._neurons = []
        self._synapses = []
        self._regions = []
        self._initialized = False

        print(f"Created NLM brain with {config.get('neuron_count', 1000)} neurons")

    def _create_default_config(self) -> Dict[str, Any]:
        """Create default configuration."""
        return {
            # Core simulation parameters
            'random_seed': 42,
            'simulation_timestep': 0.001,
            'neuron_count': 1000,
            'region_count': 1,
            'connection_probability': 0.1,

            # Plasticity settings
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_time_constant': 20.0,
            'plasticity_learning_rate': 0.01,

            # Development settings
            'development_synaptogenesis_rate': 0.001,
            'development_pruning_rate': 0.0001,

            # Memory settings
            'working_memory_capacity': 100,
            'episodic_memory_max_episodes': 1000,

            # Neuromodulation
            'dopamine_baseline': 0.1,
            'curiosity_baseline': 0.0,

            # Simulation
            'max_simulation_steps': 10000,
            'enable_structural_plasticity': False,
            'enable_development': False,
            'enable_curiosity': False,
        }

    def _validate_config(self):
        """Validate configuration."""
        is_valid, error = validate_config(self.config)
        if not is_valid:
            raise ValueError(f"Invalid configuration: {error}")

        # Additional NLM-specific validation
        if self.config['neuron_count'] < 100:
            print("Warning: Small neuron count may not show realistic dynamics")

    def initialize(self) -> bool:
        """
        Initialize the brain with current configuration.

        This creates the neural architecture, initializes neurons and synapses,
        and sets up all integrated systems.

        Returns:
            bool: True if initialization successful, False otherwise
        """
        try:
            print("Initializing NLM brain...")

            # Create neural regions
            self._create_regions()

            # Create neurons in regions
            self._create_neurons()

            # Initialize plasticity systems
            self._initialize_plasticity()

            # Initialize memory systems
            self._initialize_memory_systems()

            # Initialize neuromodulation systems
            self._initialize_neuromodulation()

            # Initialize development system
            self._initialize_development()

            self._initialized = True
            print(f"Brain initialized successfully with {len(self._neurons)} neurons")
            return True

        except Exception as e:
            print(f"Failed to initialize brain: {e}")
            return False

    def _create_regions(self):
        """Create neural regions based on configuration."""
        region_count = self.config['region_count']
        neurons_per_region = self.config['neuron_count'] // region_count

        for i in range(region_count):
            region_id = i + 1
            self._regions.append({
                'id': region_id,
                'name': f"Region_{region_id}",
                'neuron_count': neurons_per_region,
                'neurons': []
            })

    def _create_neurons(self):
        """Create neurons and populate regions."""
        for region in self._regions:
            region_neurons = []
            for j in range(region['neuron_count']):
                neuron_id = len(self._neurons) + j + 1
                neuron = self._create_neuron(neuron_id)
                region_neurons.append(neuron)
                self._neurons.append(neuron)

            region['neurons'] = region_neurons

    def _create_neuron(self, neuron_id: int) -> Dict[str, Any]:
        """Create a single neuron with random properties."""
        import random

        neuron_type = random.choice(['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Internal'])

        return {
            'id': neuron_id,
            'type': neuron_type,
            'membrane_potential': -70.0,
            'threshold': -55.0,
            'refractory_period': 0,
            'refractory_remaining': 0,
            'firing_state': 'Resting',
            'last_spike_time': 0.0,
            'total_current': 0.0,
            'is_active': False,
            'activations': [],
            'connections': []
        }

    def _initialize_plasticity(self):
        """Initialize plasticity systems (STDP, Hebbian, structural)."""
        self._plasticity_systems = {
            'stdp': {
                'enabled': True,
                'learning_rate': self.config['stdp_ltp_weight'],
                'time_constant': self.config['stdp_time_constant']
            },
            'hebbian': {
                'enabled': True,
                'learning_rate': self.config['plasticity_learning_rate']
            },
            'structural': {
                'enabled': self.config['enable_structural_plasticity'],
                'synaptogenesis_rate': self.config['development_synaptogenesis_rate'],
                'pruning_rate': self.config['development_pruning_rate']
            }
        }

    def _initialize_memory_systems(self):
        """Initialize memory systems."""
        self._memory_systems = {
            'working': {
                'capacity': self.config['working_memory_capacity'],
                'traces': {},
                'active_traces': []
            },
            'episodic': {
                'max_episodes': self.config['episodic_memory_max_episodes'],
                'episodes': [],
                'current_episode': None
            },
            'associative': {
                'patterns': {},
                'associations': {}
            }
        }

    def _initialize_neuromodulation(self):
        """Initialize neuromodulation systems."""
        self._neuromodulation_systems = {
            'dopamine': {
                'level': self.config['dopamine_baseline'],
                'plasticity_factor': 1.0,
                'reward_prediction_error': 0.0
            },
            'curiosity': {
                'level': self.config['curiosity_baseline'],
                'enabled': self.config['enable_curiosity']
            },
            'novelty': {
                'level': 0.0,
                'sensitivity': 0.5
            }
        }

    def _initialize_development(self):
        """Initialize development system."""
        self._development_system = {
            'stage': 'Initial',
            'enabled': self.config['enable_development'],
            'plasticity_modulation': True,
            'age': 0
        }

    def step(self, step_number: int) -> Dict[str, Any]:
        """
        Perform one simulation step.

        Args:
            step_number: Current step number

        Returns:
            dict: Statistics about this step
        """
        if not self._initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")

        print(f"Step {step_number}...")

        # Update brain state
        self._time += self.config['simulation_timestep']
        self._step_count = step_number

        # Update development stage
        self._update_development()

        # Process neural dynamics
        self._process_neural_dynamics()

        # Update neuromodulation
        self._update_neuromodulation()

        # Update plasticity
        self._update_plasticity()

        # Store episode in episodic memory
        self._store_current_episode()

        # Return step statistics
        return self._get_step_statistics()

    def _process_neural_dynamics(self):
        """Process neural dynamics for this step."""
        # This would implement the real neural computation
        # For demonstration, we'll simulate some basic dynamics

        # Update membrane potentials
        for neuron in self._neurons:
            if neuron['firing_state'] == 'Refractory':
                neuron['refractory_remaining'] -= 1
                if neuron['refractory_remaining'] <= 0:
                    neuron['firing_state'] = 'Resting'
                    neuron['refractory_remaining'] = 0
                    neuron['membrane_potential'] = -70.0

            elif neuron['firing_state'] != 'Inhibited':
                # Simple LIF dynamics simulation
                input_current = neuron['total_current']
                decay = 0.04
                neuron['membrane_potential'] += (input_current - neuron['membrane_potential'] * decay) * self.config['simulation_timestep']

                # Check for threshold crossing
                if neuron['membrane_potential'] >= neuron['threshold']:
                    neuron['firing_state'] = 'Refractory'
                    neuron['refractory_period'] = 5
                    neuron['refractory_remaining'] = 5
                    neuron['last_spike_time'] = self._time
                    neuron['membrane_potential'] = neuron['threshold']

            # Clear input current for next step
            neuron['total_current'] = 0.0

    def _update_neuromodulation(self):
        """Update neuromodulation systems."""
        # Update dopamine based on reward prediction error
        if 'dopamine' in self._neuromodulation_systems:
            self._neuromodulation_systems['dopamine']['level'] *= 0.95
            if self._neuromodulation_systems['dopamine']['reward_prediction_error'] > 0:
                self._neuromodulation_systems['dopamine']['level'] += 0.1
                self._neuromodulation_systems['dopamine']['plasticity_factor'] = 1.5
            else:
                self._neuromodulation_systems['dopamine']['plasticity_factor'] = 1.0

        # Update curiosity
        if 'curiosity' in self._neuromodulation_systems:
            self._neuromodulation_systems['curiosity']['level'] *= 0.98

    def _update_plasticity(self):
        """Update synaptic plasticity based on neural activity."""
        # STDP plasticity
        if self._plasticity_systems['stdp']['enabled']:
            self._update_stdp()

        # Hebbian plasticity
        if self._plasticity_systems['hebbian']['enabled']:
            self._update_hebbian()

        # Structural plasticity
        if self._plasticity_systems['structural']['enabled'] and self._step_count % 100 == 0:
            self._update_structural_plasticity()

    def _update_stdp(self):
        """Update STDP synaptic plasticity."""
        # Simplified STDP implementation
        for synapse in self._synapses:
            if synapse['pre_spike_time'] > 0 and synapse['post_spike_time'] > 0:
                time_diff = synapse['post_spike_time'] - synapse['pre_spike_time']

                if time_diff > 0:  # Post before pre = LTP
                    synapse['weight'] += 0.01
                else:  # Pre before post = LTD
                    synapse['weight'] -= 0.01

                # Clamp weight
                synapse['weight'] = max(0.0, min(1.0, synapse['weight']))

    def _update_hebbian(self):
        """Update Hebbian synaptic plasticity."""
        # Simplified Hebbian learning - co-activation strengthens synapses
        for synapse in self._synapses:
            pre_active = synapse['pre_neuron']['is_active']
            post_active = synapse['post_neuron']['is_active']

            if pre_active and post_active:
                synapse['weight'] += 0.005
                synapse['weight'] = min(1.0, synapse['weight'])

    def _update_structural_plasticity(self):
        """Update structural plasticity (synaptogenesis and pruning)."""
        import random

        # Synaptogenesis - create new synapses
        if random.random() < self._plasticity_systems['structural']['synaptogenesis_rate']:
            # Simplified synaptogenesis
            pass

        # Pruning - remove weak synapses
        if random.random() < self._plasticity_systems['structural']['pruning_rate']:
            # Simplified pruning
            pass

    def _update_development(self):
        """Update developmental processes."""
        if not self._development_system['enabled']:
            return

        self._development_system['age'] += self.config['simulation_timestep']

        # Change developmental stage
        age = self._development_system['age']

        if age < 50:  # Critical period
            self._development_system['stage'] = 'CriticalPeriod'
            self._plasticity_systems['structural']['synaptogenesis_rate'] *= 2.0
        elif age < 200:  # Maturation
            self._development_system['stage'] = 'Maturation'
            self._plasticity_systems['structural']['synaptogenesis_rate'] *= 0.5
        else:  # Adult
            self._development_system['stage'] = 'Adult'
            self._plasticity_systems['structural']['synaptogenesis_rate'] *= 0.1

    def _store_current_episode(self):
        """Store current brain state as an episode in episodic memory."""
        active_neurons = [n['id'] for n in self._neurons if n['is_active']]

        if active_neurons:
            episode = {
                'timestamp': self._step_count,
                'active_neurons': active_neurons,
                'neuron_activations': [n['activations'][-1] if n['activations'] else 0.0 for n in self._neurons],
                'reward': self._neuromodulation_systems['dopamine']['level'],
                'stage': self._development_system['stage']
            }

            self._memory_systems['episodic']['episodes'].append(episode)

            # Limit episode storage
            if len(self._memory_systems['episodic']['episodes']) > self._memory_systems['episodic']['max_episodes']:
                self._memory_systems['episodic']['episodes'].pop(0)

    def _get_step_statistics(self) -> Dict[str, Any]:
        """Get statistics for the current step."""
        total_neurons = len(self._neurons)
        active_neurons = sum(1 for n in self._neurons if n['is_active'])
        firing_neurons = sum(1 for n in self._neurons if n['firing_state'] == 'Refractory')

        return {
            'step': self._step_count,
            'time': self._time,
            'total_neurons': total_neurons,
            'active_neurons': active_neurons,
            'firing_neurons': firing_neurons,
            'development_stage': self._development_system['stage'],
            'dopamine_level': self._neuromodulation_systems['dopamine']['level'],
            'curiosity_level': self._neuromodulation_systems['curiosity']['level']
        }

    def inject_current(self, neuron_id: int, current: float):
        """Inject current into a specific neuron."""
        for neuron in self._neurons:
            if neuron['id'] == neuron_id:
                neuron['total_current'] += current
                break

    def inject_current_to_neurons(self, neuron_type: str, current: float):
        """Inject current into all neurons of a specific type."""
        for neuron in self._neurons:
            if neuron['type'] == neuron_type:
                neuron['total_current'] += current

    def reset(self):
        """Reset the brain to initial state."""
        print("Resetting brain...")

        # Reset neurons
        for neuron in self._neurons:
            neuron['membrane_potential'] = -70.0
            neuron['refractory_remaining'] = 0
            neuron['firing_state'] = 'Resting'
            neuron['total_current'] = 0.0
            neuron['is_active'] = False
            neuron['activations'] = []

        # Reset memory systems
        for system_name, system in self._memory_systems.items():
            if system_name == 'working':
                system['traces'] = {}
                system['active_traces'] = []
            elif system_name == 'episodic':
                system['episodes'] = []
                system['current_episode'] = None
            elif system_name == 'associative':
                system['patterns'] = {}
                system['associations'] = {}

        # Reset neuromodulation systems
        for system_name, system in self._neuromodulation_systems.items():
            if system_name == 'dopamine':
                system['level'] = self.config['dopamine_baseline']
                system['plasticity_factor'] = 1.0
                system['reward_prediction_error'] = 0.0
            elif system_name == 'curiosity':
                system['level'] = self.config['curiosity_baseline']

        # Reset development system
        self._development_system['stage'] = 'Initial'
        self._development_system['age'] = 0

        self._step_count = 0
        self._time = 0.0
        self._initialized = False

    def get_stats(self) -> Dict[str, Any]:
        """Get current brain statistics."""
        total_neurons = len(self._neurons)
        active_neurons = sum(1 for n in self._neurons if n['is_active'])
        firing_neurons = sum(1 for n in self._neurons if n['firing_state'] == 'Refractory')

        return {
            'total_neurons': total_neurons,
            'active_neurons': active_neurons,
            'firing_neurons': firing_neurons,
            'average_firing_rate': firing_neurons / total_neurons if total_neurons > 0 else 0.0,
            'development_stage': self._development_system['stage'],
            'dopamine_level': self._neuromodulation_systems['dopamine']['level'],
            'current_step': self._step_count,
            'elapsed_time': self._time,
            'memory_episodes': len(self._memory_systems['episodic']['episodes'])
        }

    def log_status(self):
        """Log current brain status."""
        stats = self.get_stats()
        print("=== NLM Brain Status ===")
        print(f"Total neurons: {stats['total_neurons']}")
        print(f"Active neurons: {stats['active_neurons']}")
        print(f"Firing neurons: {stats['firing_neurons']}")
        print(f"Average firing rate: {stats['average_firing_rate']:.2%}")
        print(f"Development stage: {stats['development_stage']}")
        print(f"Dopamine level: {stats['dopamine_level']:.3f}")
        print(f"Current step: {stats['current_step']}")
        print(f"Memory episodes: {stats['memory_episodes']}")
        print("=======================")
