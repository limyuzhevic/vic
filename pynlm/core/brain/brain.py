"""NLM Brain class - Main neural simulation engine."""

import time
import threading
from typing import List, Dict, Optional, Any
from enum import Enum

from .neuron import Neuron, create_neuron
from .synapse import Synapse, create_synapse
from .enums import NeuronType, DevelopmentalStage
from ..config import Config
from .exceptions import NLMException, SimulationError


class Brain:
    """Main neural network simulation engine."""
    
    def __init__(self, config: Config = None):
        """
        Initialize brain simulation.
        
        Args:
            config: Configuration object (uses default if None)
        """
        self._config = config or create_default_config()
        self._neurons = {}  # neuron_id -> Neuron
        self._synapses = {}  # synapse_id -> Synapse
        self._regions = {}  # region_id -> list of neuron_ids
        self._populations = {}  # population_id -> list of neuron_ids
        
        # Simulation state
        self._current_time = 0.0
        self._simulation_step = 0
        self._is_initialized = False
        self._is_running = False
        self._thread = None
        
        # Metrics
        self._total_spikes = 0
        self._firing_count = 0
        
        # Memory systems
        self._working_memory = None
        self._episodic_memory = None
        self._neuromodulation_systems = {}
        self._prediction_system = None
        self._cognition_systems = {}
        self._development_system = None
        
    def _random(self) -> float:
        """Generate random float between 0 and 1."""
        import random
        return random.random()
        
    def initialize(self) -> bool:
        """
        Initialize brain with current configuration.
        
        Returns:
            True if initialization successful, False otherwise
        """
        try:
            if self._is_initialized:
                self._cleanup()
            
            # Create neurons based on configuration
            neuron_count = self._config.get_or("neuron_count", 1000)
            region_count = self._config.get_or("region_count", 1)
            
            # Create regions
            for region_id in range(1, region_count + 1):
                self._regions[region_id] = []
            
            # Create neurons
            for neuron_id in range(neuron_count):
                # Assign neurons to regions
                region_id = (neuron_id % region_count) + 1
                self._neurons[neuron_id] = create_neuron(neuron_id)
                self._regions[region_id].append(neuron_id)
            
            # Create random connections based on probability
            connection_prob = self._config.get_or("connection_probability", 0.1)
            self._create_connections(connection_prob)
            
            # Initialize synapses
            for synapse in self._synapses.values():
                synapse.reset()
            
            # Initialize neurons
            for neuron in self._neurons.values():
                neuron.reset()
                neuron.initialize()

            self._is_initialized = True
            self._simulation_step = 0
            self._current_time = 0.0
            
            return True
            
        except Exception as e:
            print(f"Error initializing brain: {e}")
            return False
    
    def _create_connections(self, connection_probability: float):
        """Create random synaptic connections."""
        neuron_ids = list(self._neurons.keys())
        
        for source_id in neuron_ids:
            for dest_id in neuron_ids:
                if source_id == dest_id:
                    continue
                    
                if self._random() < connection_probability:
                    synapse_id = len(self._synapses)
                    synapse = create_synapse(synapse_id, source_id, dest_id)
                    self._synapses[synapse_id] = synapse
    
    def step(self, step: int = None, timestamp: float = None):
        """
        Advance simulation by one step.
        
        Args:
            step: Step number
            timestamp: Current time (ms)
        """
        if not self._is_initialized:
            raise SimulationError("Brain not initialized")
        
        if step is not None:
            self._simulation_step = step
        if timestamp is not None:
            self._current_time = timestamp
        else:
            self._current_time += self._config.get_or("simulation_timestep", 0.001)
        
        # Process synaptic connections
        self._process_synapses()
        
        # Update neurons
        self._update_neurons()
        
        # Update brain systems
        self._update_systems()
        
    def _process_synapses(self):
        """Process all synaptic connections."""
        for synapse in self._synapses.values():
            synapse.step(self._current_time)
            
            # Check if spike should be transmitted (simplified)
            # In a real implementation, this would check for delays and timing
            if self._random() < synapse.get_weight() * 0.1:  # Simplified transmission probability
                # Find destination neuron and inject current
                dest_id = synapse.get_destination_neuron()
                if dest_id in self._neurons:
                    if synapse.is_excitatory():
                        self._neurons[dest_id].inject_current(abs(synapse.get_weight()))
                    else:
                        self._neurons[dest_id].inject_current(-abs(synapse.get_weight()))
    
    def _update_neurons(self):
        """Update all neurons."""
        for neuron in self._neurons.values():
            neuron.step(self._current_time)
            
            # Count spikes
            if neuron.check_threshold():
                self._total_spikes += 1
                self._firing_count += 1
    
    def _update_systems(self):
        """Update brain systems (memory, neuromodulation, etc.)."""
        # Placeholder for system updates
        pass
    
    def reset(self):
        """Reset brain to initial state."""
        if self._is_initialized:
            for synapse in self._synapses.values():
                synapse.reset()
            
            for neuron in self._neurons.values():
                neuron.reset()
            
            self._total_spikes = 0
            self._firing_count = 0
            self._simulation_step = 0
            self._current_time = 0.0
    
    def save(self, filepath: str) -> bool:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file
            
        Returns:
            True if save successful, False otherwise
        """
        try:
            import pickle
            
            data = {
                'neurons': {id: n.get_state() for id, n in self._neurons.items()},
                'synapses': {id: {
                    'weight': s.get_weight(),
                    'delay': s.get_delay(),
                    'type': s.get_type().value,
                    'efficacy': s.get_efficacy()
                } for id, s in self._synapses.items()},
                'regions': self._regions,
                'populations': self._populations,
                'config': self._config,
                'simulation_step': self._simulation_step,
                'current_time': self._current_time,
                'total_spikes': self._total_spikes
            }
            
            with open(filepath, 'wb') as f:
                pickle.dump(data, f)
                
            return True
            
        except Exception as e:
            print(f"Error saving brain: {e}")
            return False
    
    def load(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            True if load successful, False otherwise
        """
        try:
            import pickle
            
            with open(filepath, 'rb') as f:
                data = pickle.load(f)
            
            # Restore neurons
            for neuron_id, neuron_state in data['neurons'].items():
                if neuron_id in self._neurons:
                    # In a real implementation, would restore neuron state
                    pass
            
            # Restore synapses
            for synapse_id, synapse_data in data['synapses'].items():
                if synapse_id in self._synapses:
                    synapse = self._synapses[synapse_id]
                    synapse.set_weight(synapse_data['weight'])
                    synapse.set_delay(synapse_data['delay'])
            
            # Restore regions and populations
            self._regions = data['regions']
            self._populations = data['populations']
            
            # Restore simulation state
            self._simulation_step = data['simulation_step']
            self._current_time = data['current_time']
            self._total_spikes = data['total_spikes']
            
            return True
            
        except Exception as e:
            print(f"Error loading brain: {e}")
            return False
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons."""
        return len(self._neurons)
        
    def get_total_synapse_count(self) -> int:
        """Get total number of synapses."""
        return len(self._synapses)
        
    def get_firing_neuron_count(self) -> int:
        """Get number of firing neurons."""
        return self._firing_count
        
    def get_average_firing_rate(self) -> float:
        """Get average firing rate (Hz)."""
        if self._simulation_step <= 0:
            return 0.0
        return (self._firing_count / self._simulation_step) * 1000.0
        
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._total_spikes
        
    def get_region_count(self) -> int:
        """Get number of regions."""
        return len(self._regions)
        
    def get_region_ids(self) -> List[int]:
        """Get region IDs."""
        return list(self._regions.keys())
        
    def get_region(self, region_id: int) -> Optional['Brain.Region']:
        """
        Get region object.
        
        Args:
            region_id: Region identifier
            
        Returns:
            Region object or None if not found
        """
        if region_id not in self._regions:
            return None
            
        return self.Region(region_id, self._regions[region_id])
        
    def add_region(self, name: str) -> int:
        """
        Add a new region.
        
        Args:
            name: Region name
            
        Returns:
            Region ID
        """
        region_id = max(self._regions.keys()) + 1 if self._regions else 1
        self._regions[region_id] = []
        return region_id
        
    def inject_current_to_neuron(self, neuron_id: int, current: float):
        """
        Inject current into specific neuron.
        
        Args:
            neuron_id: Target neuron ID
            current: Current amplitude
        """
        if neuron_id in self._neurons:
            self._neurons[neuron_id].inject_current(current)
            
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float):
        """
        Inject current into all neurons of specific type.
        
        Args:
            neuron_type: Type of neurons to target
            current: Current amplitude
        """
        for neuron in self._neurons.values():
            if neuron.get_type() == neuron_type:
                neuron.inject_current(current)
        
    # CamelCase aliases for test compatibility
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count (camelCase alias)."""
        return self.get_total_neuron_count()
        
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count (camelCase alias)."""
        return self.get_total_synapse_count()
        
    def getFiringNeuronCount(self) -> int:
        """Get firing neuron count (camelCase alias)."""
        return self.get_firing_neuron_count()
        
    def getAverageFiringRate(self) -> float:
        """Get average firing rate (camelCase alias)."""
        return self.get_average_firing_rate()
        
    def getTotalSpikeCount(self) -> int:
        """Get total spike count (camelCase alias)."""
        return self.get_total_spike_count()
        
    def getRegionCount(self) -> int:
        """Get region count (camelCase alias)."""
        return self.get_region_count()
        
    def getRegionIds(self) -> List[int]:
        """Get region IDs (camelCase alias)."""
        return self.get_region_ids()
    
    def get_working_memory(self):
        """Get working memory system."""
        return self._working_memory
        
    def get_episodic_memory(self):
        """Get episodic memory system."""
        return self._episodic_memory
        
    def get_dopamine(self):
        """Get dopamine neuromodulation system."""
        return self._neuromodulation_systems.get('dopamine')
        
    def get_curiosity(self):
        """Get curiosity neuromodulation system."""
        return self._neuromodulation_systems.get('curiosity')
        
    def get_prediction_system(self):
        """Get prediction system."""
        return self._prediction_system
        
    def get_planner(self):
        """Get planner cognition system."""
        return self._cognition_systems.get('planner')
        
    def get_concept_formation(self):
        """Get concept formation cognition system."""
        return self._cognition_systems.get('concept_formation')
        
    def get_attention(self):
        """Get attention cognition system."""
        return self._cognition_systems.get('attention')
        
    def get_development_system(self):
        """Get development system."""
        return self._development_system
        
    def enable_plasticity(self, hebbian: bool = False, stdp: bool = False, 
                          reward_modulated: bool = False):
        """
        Enable plasticity for all synapses.
        
        Args:
            hebbian: Enable Hebbian learning
            stdp: Enable STDP
            reward_modulated: Enable reward-modulated learning
        """
        for synapse in self._synapses.values():
            synapse.enable_plasticity(hebbian, stdp, reward_modulated)
    
    def get_development_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        # Placeholder implementation
        return DevelopmentalStage.ADULT
    
    def produce_action(self):
        """
        Produce motor action based on current brain state.
        
        Returns:
            Action object
        """
        # Placeholder implementation - return simple action
        return self.Action()
    
    def receive_sensory_input(self, sensory_input):
        """
        Receive and process sensory input.
        
        Args:
            sensory_input: Sensory input data
        """
        # Placeholder implementation
        pass
    
    def log_status(self):
        """Print brain status and statistics."""
        print(f"Brain Status:")
        print(f"  Neurons: {self.get_total_neuron_count()}")
        print(f"  Synapses: {self.get_total_synapse_count()}")
        print(f"  Firing neurons: {self.get_firing_neuron_count()}")
        print(f"  Total spikes: {self.get_total_spike_count()}")
        print(f"  Average firing rate: {self.get_average_firing_rate():.2f} Hz")
        print(f"  Regions: {self.get_region_count()}")
        print(f"  Current time: {self._current_time:.3f} ms")
        print(f"  Simulation step: {self._simulation_step}")
    
    class Region:
        """Brain region container."""
        
        def __init__(self, region_id: int, neuron_ids: List[int]):
            self._region_id = region_id
            self._neuron_ids = neuron_ids
            
        def get_id(self) -> int:
            return self._region_id
            
        def get_neuron_ids(self) -> List[int]:
            return self._neuron_ids.copy()
            
        def get_neuron_count(self) -> int:
            return len(self._neuron_ids)
    
    class Action:
        """Motor action produced by brain."""
        
        def __init__(self, action_type: str = "none", value: float = 0.0):
            self._action_type = action_type
            self._value = value
            
        def get_type(self) -> str:
            return self._action_type
            
        def get_value(self) -> float:
            return self._value

    # Helper functions
    def create_default_config():
        """Create default configuration (placeholder)."""
        # This should import the actual Config class
        return Config()