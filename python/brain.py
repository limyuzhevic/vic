"""
Brain module - Core brain functionality for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import math
import time

from .pynlm import (
    Brain as BrainCore, Config as ConfigCore, RandomGenerator, SimulationClock, Logger,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    Neuron, Synapse, NeuralRegion, NeuralPopulation, MembranePotential, 
    SynapticWeight, FiringRate, FiringState, NeuronType, RegionId, PopulationId,
    NeuralDynamics, IntegrateAndFireDynamics, HodgkinHuxleyDynamics, 
    FitzHughNagumoDynamics
)

__all__ = [
    'Brain', 'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'PlasticityFlags', 'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'NeuronType', 'RegionId', 'PopulationId', 'NeuralDynamics', 'IntegrateAndFireDynamics',
    'HodgkinHuxleyDynamics', 'FitzHughNagumoDynamics',
]

class Brain:
    """Enhanced brain simulation with high-level interface and convenience methods"""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None, neuron_count: Optional[int] = None, region_count: Optional[int] = None):
        """Initialize Brain
        
        Args:
            config: Configuration dictionary
            neuron_count: Number of neurons (overrides config)
            region_count: Number of regions (overrides config)
        """
        self._config = config or {}
        self._neuron_count = neuron_count or self._config.get('neuron_count', 1000)
        self._region_count = region_count or self._config.get('region_count', 2)
        
        # Initialize core brain
        self._brain = BrainCore(self._neuron_count, self._region_count)
        
        # Configuration
        self._config_obj = ConfigCore()
        self._apply_config()
        
        # Performance tracking
        self._step_times = []
        self._max_history = 1000
    
    def _apply_config(self):
        """Apply configuration to brain"""
        timestep = self._config.get('simulation_timestep', 0.001)
        self._brain.setTimestep(timestep)
        
        # Set random seed
        seed = self._config.get('random_seed', 42)
        self._brain.setSeed(seed)
        
        # Initialize brain
        self._brain.initialize()
    
    def step(self, step: SimulationStep, timestep: Optional[TimestepDuration] = None) -> Dict[str, Any]:
        """Advance brain simulation by one timestep
        
        Args:
            step: Current simulation step
            timestep: Optional timestep (uses configured timestep if None)
            
        Returns:
            Dictionary with step results
        """
        start_time = time.time()
        
        # Step brain
        self._brain.step(step)
        
        # Calculate performance metrics
        step_time = time.time() - start_time
        self._step_times.append(step_time)
        if len(self._step_times) > self._max_history:
            self._step_times.pop(0)
        
        # Gather results
        result = {
            'step': step,
            'neurons_active': self.get_firing_neuron_count(),
            'average_firing_rate': self.get_average_firing_rate(),
            'step_time': step_time,
            'avg_step_time': sum(self._step_times) / len(self._step_times),
            'neuron_count': self._neuron_count,
            'region_count': self._region_count,
        }
        
        return result
    
    def initialize(self):
        """Initialize the brain"""
        self._brain.initialize()
    
    def reset(self):
        """Reset brain to initial state"""
        self._brain.reset()
    
    def get_neuron_count(self) -> int:
        """Get number of neurons
        
        Returns:
            Number of neurons
        """
        return self._neuron_count
    
    def get_region_count(self) -> int:
        """Get number of regions
        
        Returns:
            Number of regions
        """
        return self._region_count
    
    def get_firing_neuron_count(self) -> int:
        """Get number of firing neurons
        
        Returns:
            Number of firing neurons
        """
        return self._brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate
        
        Returns:
            Average firing rate (Hz)
        """
        return self._brain.getAverageFiringRate()
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count
        
        Returns:
            Total neuron count
        """
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count
        
        Returns:
            Total synapse count
        """
        return self._brain.getTotalSynapseCount()
    
    def set_dynamics(self, dynamics_type: str, **kwargs):
        """Set neural dynamics type
        
        Args:
            dynamics_type: Type of dynamics ('integrate_and_fire', 'hodgkin_huxley', 'fhx')
            **kwargs: Parameters for dynamics
        """
        if dynamics_type == 'integrate_and_fire':
            dynamics = IntegrateAndFireDynamics()
            if 'membrane_time_constant' in kwargs:
                dynamics.setMembraneTimeConstant(kwargs['membrane_time_constant'])
        elif dynamics_type == 'hodgkin_huxley':
            dynamics = HodgkinHuxleyDynamics()
        elif dynamics_type == 'fhx':
            dynamics = FitzHughNagumoDynamics()
        else:
            raise ValueError(f"Unknown dynamics type: {dynamics_type}")
        
        self._brain.setDynamics(dynamics)
    
    def add_region(self, region_id: RegionId, neuron_count: int = 0, neuron_type: NeuronType = NeuronType::Excitatory):
        """Add a neural region
        
        Args:
            region_id: Region identifier
            neuron_count: Number of neurons in region
            neuron_type: Type of neurons
        """
        if neuron_count > 0:
            self._brain.addRegion(region_id, neuron_count, neuron_type)
    
    def get_regions(self) -> List['NeuralRegion']:
        """Get all neural regions
        
        Returns:
            List of neural regions
        """
        # This would require adding a method to the core Brain class
        # For now, return empty list as placeholder
        return []
    
    def connect_regions(self, source_region: RegionId, dest_region: RegionId, connection_probability: float = 0.1):
        """Connect neurons between regions
        
        Args:
            source_region: Source region
            dest_region: Destination region
            connection_probability: Probability of connection
        """
        # This would require adding methods to the core Brain class
        # For now, return without doing anything as placeholder
        pass
    
    def run_simulation(self, steps: int, visualize: bool = False) -> List[Dict[str, Any]]:
        """Run complete simulation
        
        Args:
            steps: Number of simulation steps
            visualize: Whether to enable visualization
            
        Returns:
            List of step results
        """
        results = []
        
        for step in range(steps):
            result = self.step(step)
            results.append(result)
            
            if visualize:
                self.visualize(step)
        
        return results
    
    def visualize(self, step: SimulationStep):
        """Visualize current brain state
        
        Args:
            step: Current simulation step
        """
        # This would require visualization interface
        # For now, return without doing anything as placeholder
        pass
    
    def get_state(self) -> Dict[str, Any]:
        """Get current brain state
        
        Returns:
            Dictionary with brain state information
        """
        return {
            'neurons_active': self.get_firing_neuron_count(),
            'average_firing_rate': self.get_average_firing_rate(),
            'total_neuron_count': self.get_total_neuron_count(),
            'total_synapse_count': self.get_total_synapse_count(),
            'neuron_count': self._neuron_count,
            'region_count': self._region_count,
        }
    
    def get_config(self) -> Dict[str, Any]:
        """Get brain configuration
        
        Returns:
            Configuration dictionary
        """
        return self._config.copy()
    
    def save_results(self, filepath: str) -> bool:
        """Save brain results to file
        
        Args:
            filepath: Path to save results
            
        Returns:
            True if saved successfully
        """
        # This would require adding a save method to the core Brain class
        # For now, return False as placeholder
        print(f"Warning: save_results not implemented in core Brain class")
        return False

# Convenience functions
def create_brain(neuron_count: int = 1000, region_count: int = 2, **kwargs) -> Brain:
    """Create a brain with specified parameters
    
    Args:
        neuron_count: Number of neurons
        region_count: Number of regions
        **kwargs: Additional configuration parameters
        
    Returns:
        Brain instance
    """
    config = kwargs.copy()
    config['neuron_count'] = neuron_count
    config['region_count'] = region_count
    return Brain(config)

class NeuronUtils:
    """Utility functions for neuron operations"""
    
    @staticmethod
    def create_neuron(id: NeuronId, neuron_type: NeuronType = NeuronType::Excitatory) -> 'Neuron':
        """Create a neuron
        
        Args:
            id: Neuron identifier
            neuron_type: Type of neuron
            
        Returns:
            Neuron instance
        """
        # This would require access to the core Neuron constructor
        # For now, return None as placeholder
        print(f"Warning: Neuron creation not directly exposed in Python bindings")
        return None
    
    @staticmethod
    def create_region(neuron_ids: List[NeuronId]) -> 'NeuralRegion':
        """Create a neural region from neuron IDs
        
        Args:
            neuron_ids: List of neuron IDs
            
        Returns:
            NeuralRegion instance
        """
        # This would require access to the core NeuralRegion constructor
        # For now, return None as placeholder
        print(f"Warning: NeuralRegion creation not directly exposed in Python bindings")
        return None
    
    @staticmethod
    def calculate_connection_probability(neuron1_type: NeuronType, neuron2_type: NeuronType, 
                                       distance: float = 1.0, weight: float = 1.0) -> float:
        """Calculate connection probability between neurons
        
        Args:
            neuron1_type: Type of first neuron
            neuron2_type: Type of second neuron
            distance: Distance between neurons
            weight: Connection weight
            
        Returns:
            Connection probability
        """
        # Default probability calculation
        base_prob = 0.1
        
        if neuron1_type == NeuronType::Excitatory and neuron2_type == NeuronType::Excitatory:
            return min(1.0, base_prob * 1.5 / (distance + 0.1))
        elif neuron1_type == NeuronType::Inhibitory and neuron2_type == NeuronType::Excitatory:
            return min(1.0, base_prob * 2.0 / (distance + 0.1))
        else:
            return min(1.0, base_prob / (distance + 0.1))
