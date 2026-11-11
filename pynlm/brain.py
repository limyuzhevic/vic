# Brain module - Enhanced Python interface for the NLM Brain
"""
Enhanced Python interface for the NLM Brain class with additional convenience methods
and better error handling.
"""

from .types import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType, DevelopmentalStage
from typing import List, Optional, Dict, Any
import numpy as np

# Try to import C++ bindings
try:
    from ._pynlm import createBrain as _createBrain
    from ._pynlm import Neuron, Synapse, NeuralRegion
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False
    
    # Create mock classes for testing/development
    class Neuron:
        pass
        
    class Synapse:
        pass
        
    class NeuralRegion:
        pass

class Brain:
    """Enhanced Python interface for NLM Brain simulation.
    
    This class provides a more Pythonic interface to the C++ Brain class,
    with additional convenience methods, better error handling, and
    extended functionality for common use cases.
    """
    
    def __init__(self, config=None):
        """Initialize brain with configuration.
        
        Args:
            config: Configuration object. If None, creates default config.
        """
        if config is None:
            config = createDefaultConfig()
        
        if CPLUSPLUS_AVAILABLE and _createBrain is not None:
            self._brain = _createBrain(config)
        else:
            # Create mock brain for testing
            self._brain = MockBrain(config)
        
        self._config = config
    
    def initialize(self):
        """Initialize the brain simulation."""
        return self._brain.initialize()
    
    def step(self, current_step):
        """Perform a simulation step.
        
        Args:
            current_step: Current simulation step number.
        """
        return self._brain.step(current_step)
    
    def step_with_time(self, current_step, current_time):
        """Perform a simulation step with timestamp.
        
        Args:
            current_step: Current simulation step number.
            current_time: Current simulation time.
        """
        return self._brain.step(current_step, current_time)
    
    def inject_current(self, neuron_id, current):
        """Inject current into a specific neuron.
        
        Args:
            neuron_id: ID of neuron to inject current into.
            current: Current value to inject.
        """
        return self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_by_type(self, neuron_type, current):
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to inject into.
            current: Current value to inject.
        """
        return self._brain.injectCurrentToNeurons(neuron_type, current)
    
    def produce_action(self):
        """Produce motor action based on neural activity.
        
        Returns:
            Action object representing the produced action.
        """
        return self._brain.produceAction()
    
    def reset(self):
        """Reset brain state."""
        return self._brain.reset()
    
    def save(self, filepath):
        """Save brain state to file.
        
        Args:
            filepath: Path to save file.
        """
        return self._brain.save(filepath)
    
    def load(self, filepath):
        """Load brain state from file.
        
        Args:
            filepath: Path to load file.
        """
        return self._brain.load(filepath)
    
    # Convenience properties
    
    @property
    def region_count(self):
        """Get number of regions."""
        return self._brain.getRegionCount()
    
    @property
    def total_neuron_count(self):
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    @property
    def total_synapse_count(self):
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    @property
    def active_neuron_count(self):
        """Get count of active neurons."""
        return self._brain.getActiveNeuronCount()
    
    @property
    def firing_neuron_count(self):
        """Get count of firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    @property
    def average_firing_rate(self):
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
    
    @property
    def excitation_inhibition_ratio(self):
        """Get excitation/inhibition balance ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    @property
    def total_spike_count(self):
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    @property
    def developmental_stage(self):
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    @property
    def config(self):
        """Get configuration object."""
        return self._config
    
    def add_region(self, name=""):
        """Add a new neural region.
        
        Args:
            name: Optional name for the region.
        """
        return self._brain.addRegion(name)
    
    def get_region(self, region_id):
        """Get a region by ID.
        
        Args:
            region_id: Region ID to retrieve.
            
        Returns:
            NeuralRegion object or None if not found.
        """
        return self._brain.getRegion(region_id)
    
    def get_region_ids(self):
        """Get all region IDs.
        
        Returns:
            List of region IDs.
        """
        return self._brain.getRegionIds()
    
    def get_regions(self):
        """Get all regions.
        
        Returns:
            List of regions.
        """
        return self._brain.getRegions()
    
    def log_status(self):
        """Log brain status information."""
        return self._brain.logStatus()
    
    def get_neuron_population_summary(self):
        """Get summary statistics of neuron populations.
        
        Returns:
            Dictionary with population statistics.
        """
        summary = {
            'total_neurons': self.total_neuron_count,
            'active_neurons': self.active_neuron_count,
            'firing_neurons': self.firing_neuron_count,
            'average_firing_rate': self.average_firing_rate,
            'e_i_ratio': self.excitation_inhibition_ratio,
            'total_spikes': self.total_spike_count,
        }
        
        # Add region-specific statistics
        for region_id in self.get_region_ids():
            region = self.get_region(region_id)
            if region:
                summary[f'region_{region_id}'] = {
                    'neuron_count': region.getNeuronCount(),
                    'active_count': region.getActiveNeuronCount(),
                    'firing_count': region.getFiringNeuronCount(),
                }
        
        return summary
    
    def simulate_episode(self, steps, sensory_inputs=None):
        """Simulate an episode with optional sensory inputs.
        
        Args:
            steps: Number of steps to simulate.
            sensory_inputs: Optional list of sensory inputs for each step.
            
        Returns:
            List of simulation results for each step.
        """
        results = []
        self.initialize()
        
        for step in range(steps):
            step_result = {
                'step': step,
                'time': step * self._config.getOr('simulation_timestep', 0.001),
                'neuron_count': self.total_neuron_count,
                'active_count': self.active_neuron_count,
                'firing_count': self.firing_neuron_count,
                'spike_count': self.total_spike_count,
                'action': None,
            }
            
            # Inject sensory input if provided
            if sensory_inputs and step < len(sensory_inputs):
                for neuron_id, current in sensory_inputs[step].items():
                    self.inject_current(neuron_id, current)
            
            # Perform step
            self.step(step)
            
            # Produce action
            action = self.produce_action()
            step_result['action'] = action
            
            # Get action result
            if hasattr(self._brain, 'getActionResult'):
                step_result['action_result'] = self._brain.getActionResult()
            
            results.append(step_result)
        
        return results
    
    def __repr__(self):
        """String representation of the brain."""
        return f"<Brain: {self.total_neuron_count} neurons, {self.region_count} regions, {self.total_spike_count} spikes>"


# Mock brain class for testing when C++ bindings are not available
class MockBrain:
    def __init__(self, config=None):
        self.config = config or {}
        self.total_neurons = 1000
        self.total_synapses = 5000
        self.total_spikes = 0
        self.active_neurons = 100
        self.firing_neurons = 50
        
    def initialize(self):
        return True
        
    def step(self, step):
        self.total_spikes += 10
        self.active_neurons = 50 + int(np.random.random() * 50)
        self.firing_neurons = 20 + int(np.random.random() * 30)
        
    def getTotalNeuronCount(self):
        return self.total_neurons
        
    def getTotalSynapseCount(self):
        return self.total_synapses
        
    def getTotalSpikeCount(self):
        return self.total_spikes
        
    def getActiveNeuronCount(self):
        return self.active_neurons
        
    def getFiringNeuronCount(self):
        return self.firing_neurons
        
    def getAverageFiringRate(self):
        return self.total_spikes / max(self.total_neurons, 1) * 1000
        
    def injectCurrent(self, neuron_id, current):
        pass
        
    def injectCurrentToNeurons(self, neuron_type, current):
        pass
        
    def produceAction(self):
        from .motor import create_action
        return create_action()
        
    def reset(self):
        self.total_spikes = 0
        
    def save(self, filepath):
        return False
        
    def load(self, filepath):
        return False
        
    def addRegion(self, name=""):
        return None
        
    def getRegion(self, region_id):
        return None
        
    def getRegionCount(self):
        return 0
        
    def getRegionIds(self):
        return []
        
    def getRegions(self):
        return []
        
    def getActiveNeuronCount(self):
        return self.active_neurons
        
    def getFiringNeuronCount(self):
        return self.firing_neurons
        
    def getExcitationInhibitionRatio(self):
        return 1.0
        
    def getDevelopmentalStage(self):
        from .types import DevelopmentalStage
        return DevelopmentalStage.Adult
        
    def logStatus(self):
        pass


# Re-export for convenience
from . import _pynlm
Brain.__base__ = _pynlm.Brain