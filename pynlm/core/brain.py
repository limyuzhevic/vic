# NLM Python Brain Implementation

import random
from typing import Dict, Any, Optional, List
from .neural_types import NeuronType, DevelopmentalStage

class Neuron:
    """Single neuron in the NLM brain."""
    
    def __init__(self, neuron_id: int):
        self._neuron_id = neuron_id
        self._state = {
            'membrane_potential': -70.0,
            'threshold': -55.0,
            'resting_potential': -70.0,
            'reset_potential': -70.0,
            'leak_conductance': 10.0,
            'firing_rate': 0.0,
            'refractory_remaining': 0,
            'refractory_period': 5,
            'adaptation_variable': 0.0,
            'last_spike_time': -1.0,
            'is_firing': False,
            'is_refractory': False
        }
    
    @property
    def neuron_id(self) -> int:
        return self._neuron_id
    
    @property
    def membrane_potential(self) -> float:
        return self._state['membrane_potential']
    
    @membrane_potential.setter
    def membrane_potential(self, value: float):
        self._state['membrane_potential'] = value
    
    @property
    def threshold(self) -> float:
        return self._state['threshold']
    
    @threshold.setter
    def threshold(self, value: float):
        self._state['threshold'] = value
    
    @property
    def is_firing(self) -> bool:
        return self._state['is_firing']
    
    @property
    def is_refractory(self) -> bool:
        return self._state['is_refractory']
    
    @property
    def firing_rate(self) -> float:
        return self._state['firing_rate']
    
    def inject_current(self, current: float) -> None:
        """Inject current into the neuron."""
        self._state['membrane_potential'] += current
    
    def step(self, current_time: float, dt: float) -> bool:
        """Step the neuron one simulation step. Returns True if neuron fired."""
        # Simple LIF neuron dynamics
        if self._state['is_refractory']:
            self._state['refractory_remaining'] -= 1
            if self._state['refractory_remaining'] <= 0:
                self._state['is_refractory'] = False
                self._state['membrane_potential'] = self._state['reset_potential']
            return False
        
        # Membrane potential decay
        self._state['membrane_potential'] *= (1.0 - dt / 20.0)  # tau = 20ms
        
        # Check if neuron fires
        if self._state['membrane_potential'] >= self._state['threshold']:
            self._state['membrane_potential'] = self._state['reset_potential']
            self._state['is_firing'] = True
            self._state['is_refractory'] = True
            self._state['refractory_remaining'] = self._state['refractory_period']
            self._state['last_spike_time'] = current_time
            return True
        
        self._state['is_firing'] = False
        return False
    
    def reset(self) -> None:
        """Reset neuron to initial state."""
        self._state['membrane_potential'] = self._state['resting_potential']
        self._state['is_firing'] = False
        self._state['is_refractory'] = False
        self._state['refractory_remaining'] = 0
        self._state['adaptation_variable'] = 0.0
        self._state['last_spike_time'] = -1.0

class Region:
    """Brain region containing neurons."""
    
    def __init__(self, region_id: int):
        self._region_id = region_id
        self._neurons = []
    
    @property
    def region_id(self) -> int:
        return self._region_id
    
    def add_neuron(self, neuron: Neuron) -> None:
        """Add a neuron to this region."""
        self._neurons.append(neuron)
    
    def get_all_neurons(self) -> List[Neuron]:
        """Get all neurons in this region."""
        return self._neurons.copy()
    
    def get_synapses(self) -> List[Any]:
        """Get all synapses in this region (mock implementation)."""
        return []

class Brain:
    """Main NLM brain simulation."""
    
    def __init__(self, config: Optional['Config'] = None):
        from .config import Config as ConfigClass
        
        if config is None:
            config = ConfigClass.create_default_config()
        self._config = config
        
        self._regions = {}
        self._neurons = []
        self._total_spikes = 0
        self._firing_neurons = 0
        self._steps = 0
        
        # Initialize brain
        self._initialize()
    
    def _initialize(self) -> None:
        """Initialize brain with neurons and regions."""
        # Create regions
        region_count = self._config.get('region_count', 2)
        for i in range(region_count):
            region = Region(i)
            self._regions[i] = region
        
        # Create neurons and distribute across regions
        neuron_count = self._config.get('neuron_count', 1000)
        neurons_per_region = neuron_count // region_count
        
        neuron_id = 0
        for region_id, region in self._regions.items():
            neurons_in_this_region = neurons_per_region
            if region_id == region_count - 1:  # Last region gets remaining neurons
                neurons_in_this_region = neuron_count - (region_count - 1) * neurons_per_region
            
            for _ in range(neurons_in_this_region):
                neuron = Neuron(neuron_id)
                region.add_neuron(neuron)
                self._neurons.append(neuron)
                neuron_id += 1
    
    def initialize(self) -> bool:
        """Initialize the brain."""
        # Reset random seed
        random.seed(self._config.get('random_seed', 42))
        
        # Reset all neurons
        for neuron in self._neurons:
            neuron.reset()
        
        self._total_spikes = 0
        self._firing_neurons = 0
        self._steps = 0
        
        return True
    
    def step(self, step_number: int, time: Optional[float] = None) -> None:
        """Run one simulation step."""
        if time is None:
            time = step_number * self._config.get('simulation_timestep', 0.001)
        
        # Simple simulation: inject random current and step neurons
        for neuron in self._neurons:
            # Inject random current (simulating sensory input)
            if random.random() < 0.1:  # 10% chance of input
                current = random.uniform(0, 50)
                neuron.inject_current(current)
            
            # Step neuron
            if neuron.step(time, self._config.get('simulation_timestep', 0.001)):
                self._total_spikes += 1
                if neuron.is_firing:
                    self._firing_neurons += 1
        
        self._steps = step_number + 1
    
    def reset(self) -> None:
        """Reset the brain to initial state."""
        self.initialize()
    
    @property
    def total_neuron_count(self) -> int:
        """Get total number of neurons."""
        return len(self._neurons)
    
    @property
    def total_synapse_count(self) -> int:
        """Get total number of synapses."""
        return len(self._neurons) * 2  # Approximate: 2 synapses per neuron
    
    @property
    def firing_neuron_count(self) -> int:
        """Get number of firing neurons."""
        return self._firing_neurons
    
    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate."""
        if self._steps == 0:
            return 0.0
        return (self._total_spikes / self._steps) * 1000.0  # Convert to Hz
    
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get excitation-inhibition ratio."""
        # Simple approximation based on neuron types
        return 1.5  # Typical E/I ratio
    
    @property
    def developmental_stage(self) -> DevelopmentalStage:
        """Get developmental stage."""
        # Simple developmental progression based on steps
        if self._steps < 1000:
            return DevelopmentalStage.INITIAL
        elif self._steps < 5000:
            return DevelopmentalStage.CRITICAL_PERIOD
        elif self._steps < 10000:
            return DevelopmentalStage.MATURATION
        elif self._steps < 50000:
            return DevelopmentalStage.ADULT
        else:
            return DevelopmentalStage.AGING
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
        try:
            import json
            data = {
                'total_spikes': self._total_spikes,
                'firing_neurons': self._firing_neurons,
                'steps': self._steps,
                'neuron_count': self.total_neuron_count,
                'region_count': len(self._regions)
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception:
            return False
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
        try:
            import json
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            self._total_spikes = data.get('total_spikes', 0)
            self._firing_neurons = data.get('firing_neurons', 0)
            self._steps = data.get('steps', 0)
            
            return True
        except Exception:
            return False
    
    def add_region(self, name: str) -> int:
        """Add a new brain region."""
        region_id = len(self._regions)
        region = Region(region_id)
        self._regions[region_id] = region
        return region_id
    
    def get_region(self, region_id: int) -> Optional[Region]:
        """Get a brain region by ID."""
        return self._regions.get(region_id)
    
    def log_status(self) -> None:
        """Log brain status (for debugging)."""
        print(f"Brain Status:")
        print(f"  Neurons: {self.total_neuron_count}")
        print(f"  Synapses: {self.total_synapse_count}")
        print(f"  Firing neurons: {self.firing_neuron_count}")
        print(f"  Average firing rate: {self.average_firing_rate:.2f} Hz")
        print(f"  E/I ratio: {self.excitation_inhibition_ratio:.2f}")
        print(f"  Developmental stage: {self.developmental_stage.name}")
        print(f"  Total spikes: {self._total_spikes}")
        print(f"  Simulation steps: {self._steps}")

# Module-level functions for compatibility
def createBrain(config: Optional['Config'] = None) -> Brain:
    """Create a new brain."""
    return Brain(config)