"""
Core NLM classes with enhanced Python interfaces and error handling
"""

import json
import os
from typing import Dict, List, Optional, Any, Union
from pathlib import Path

from .exceptions import NLMError, NLMConfigurationError, NLMFileError

class Config:
    """Enhanced Python interface for NLM configuration"""
    
    def __init__(self, config_ptr=None):
        self._config_ptr = config_ptr
        
    @staticmethod
    def create_default():
        """Create a default configuration"""
        return Config()
        
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value with validation"""
        if not key:
            raise NLMConfigurationError("Configuration key cannot be empty")
        if value is None:
            raise NLMConfigurationError(f"Cannot set {key} to None")
            
    def get(self, key: str, default=None):
        """Get a configuration value"""
        
    def has(self, key: str) -> bool:
        """Check if configuration key exists"""
        
    def get_keys(self) -> List[str]:
        """Get all configuration keys"""
        
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from JSON file"""
        
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to JSON file"""
        
    def clear(self) -> None:
        """Clear all configuration entries"""
        
    def summary(self) -> str:
        """Get a summary of the configuration"""
        
    def set_batch(self, config_dict: Dict[str, Any]) -> None:
        """Set multiple configuration values at once"""
        for key, value in config_dict.items():
            if not isinstance(key, str):
                raise NLMConfigurationError(f"Configuration key must be string, got {type(key)}")
            self.set(key, value)
            
    def validate(self) -> bool:
        """Validate configuration values"""
        
    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"
class Brain:
    """Enhanced Python interface for NLM brain simulation"""
    
    def __init__(self, config=None):
        self._brain_ptr = None
        self._config = config or Config.create_default()
        
    @property
    def config(self) -> Config:
        """Get the brain's configuration"""
        
    def initialize(self) -> bool:
        """Initialize the brain with configuration"""
        
    def step(self, step: int, time: Optional[float] = None) -> None:
        """Perform a simulation step"""
        
    def run_simulation(self, num_steps: int, timestep: float = 0.001, 
                      callback=None) -> Dict[str, Any]:
        """Run a complete simulation with optional callback"""
        
    def save(self, filepath: str) -> bool:
        """Save brain state to file"""
        
    def load(self, filepath: str) -> bool:
        """Load brain state from file"""
        
    def reset(self) -> None:
        """Reset brain to initial state"""
        
    # Statistics methods
    def get_total_neuron_count(self) -> int:
        """Get total neuron count"""
        
    def get_total_synapse_count(self) -> int:
        """Get total synapse count"""
        
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons"""
        
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons"""
        
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons"""
        
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio"""
        
    def get_total_spike_count(self) -> int:
        """Get total spike count"""
        
    def get_pending_spike_event_count(self) -> int:
        """Get count of pending spike events"""
        
    def get_developmental_stage(self) -> str:
        """Get current developmental stage"""
        
    def set_developmental_stage(self, stage: str) -> None:
        """Set developmental stage"""
        
    # Neuromodulation access
    def get_dopamine(self):
        """Get dopamine neuromodulator"""
        from .neuromodulation import Dopamine
        # This would return a Dopamine object wrapping the C++ implementation
        return Dopamine()
        
    def get_curiosity(self):
        """Get curiosity neuromodulator"""
        from .neuromodulation import Curiosity
        return Curiosity()
        
    def get_novelty(self):
        """Get novelty neuromodulator"""
        from .neuromodulation import Novelty
        return Novelty()
        
    def get_prediction_error(self):
        """Get prediction error signal"""
        from .neuromodulation import PredictionError
        return PredictionError()
        
    # Neuromodulation access
    def get_dopamine(self):
        """Get dopamine neuromodulator"""
        from .neuromodulation import Dopamine
        # This would return a Dopamine object wrapping the C++ implementation
        return Dopamine()
        
    def get_curiosity(self):
        """Get curiosity neuromodulator"""
        from .neuromodulation import Curiosity
        return Curiosity()
        
    def get_novelty(self):
        """Get novelty neuromodulator"""
        from .neuromodulation import Novelty
        return Novelty()
        
    def get_prediction_error(self):
        """Get prediction error signal"""
        from .neuromodulation import PredictionError
        return PredictionError()
        
    # Memory system access
    def get_working_memory(self):
        """Get working memory system"""
        from .memory import WorkingMemory
        return WorkingMemory()
        
    def get_episodic_memory(self):
        """Get episodic memory system"""
        from .memory import EpisodicMemory
        return EpisodicMemory()
        
    def get_associative_memory(self):
        """Get associative memory system"""
        from .memory import AssociativeMemory
        return AssociativeMemory()
        
    # Prediction system access
    def get_prediction_system(self):
        """Get prediction system"""
        from .prediction import PredictionSystem
        return PredictionSystem()
        
    def get_planner(self):
        """Get neural planner"""
        from .cognition import NeuralPlanner
        return NeuralPlanner()
        
    def get_concept_formation(self):
        """Get concept formation system"""
        from .cognition import ConceptFormation
        return ConceptFormation()
        
    def get_attention(self):
        """Get attentional selection system"""
        from .cognition import AttentionalSelection
        return AttentionalSelection()
        
    # Development system access
    def get_development_system(self):
        """Get development system"""
        from .development import DevelopmentSystem
        return DevelopmentSystem()
        
    # Advanced plasticity system access
    def get_stdp(self):
        """Get STDP plasticity system"""
        from .plasticity import STDP
        return STDP()
        
    def get_hebbian(self):
        """Get Hebbian plasticity system"""
        from .plasticity import Hebbian
        return Hebbian()
        
    def get_structural_plasticity(self):
        """Get structural plasticity system"""
        from .plasticity import StructuralPlasticity
        return StructuralPlasticity()
        
    def get_spike_system(self):
        """Get spike system"""
        from .dynamics import SpikeSystem
        return SpikeSystem()
        
    def get_random_generator(self):
        """Get random number generator"""
        # This would return a Python random generator
        import random
        return random.Random()
        
    # State capture/restore
    def capture_state(self) -> Dict[str, Any]:
        """Capture complete brain state as Python dictionary"""
        
    def restore_state(self, state: Dict[str, Any]) -> bool:
        """Restore brain state from Python dictionary"""
        
    # Sensory input methods
    def receive_sensory_input(self, sensory_input) -> None:
        """Inject sensory input into the brain"""
        
    def inject_current(self, neuron_id, current: float) -> None:
        """Inject current into a specific neuron"""
        
    def inject_current_to_neurons(self, neuron_type: str, current: float) -> None:
        """Inject current into all neurons of a specific type"""
        
    # Action production
    def produce_action(self):
        """Produce motor action based on neural activity"""
        
    # Region management
    def add_region(self, name: str = ""):
        """Add a new neural region"""
        
    def get_region(self, region_id):
        """Get a region by ID"""
        
    def get_region_count(self) -> int:
        """Get the number of regions"""
        
    def get_region_ids(self) -> List[int]:
        """Get all region IDs"""
        
    # Inter-region connection management
    def add_inter_region_connection(self, source_id: int, target_id: int, 
                                  weight: float = 0.0, delay: int = 1) -> None:
        """Add a connection between regions"""
        
    def remove_inter_region_connection(self, source_id: int, target_id: int) -> None:
        """Remove a connection between regions"""
        
    # Statistics and logging
    def log_status(self) -> None:
        """Log brain status and statistics"""
        
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics"""
        
    def __repr__(self) -> str:
        return f"<Brain: {self.get_total_neuron_count()} neurons>"

# Special neuron and synapse ID types
class NeuronId:
    """Python wrapper for NeuronId"""
    def __init__(self, value: int = 0):
        self.value = value
        
    def index(self) -> int:
        """Get the index of this neuron ID"""
        
    def __eq__(self, other):
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
        
    def __hash__(self):
        return hash(self.value)
        
    def __repr__(self):
        return f"<NeuronId: {self.value}>"
class SynapseId:
    """Python wrapper for SynapseId"""
    def __init__(self, value: int = 0):
        self.value = value
        
    def index(self) -> int:
        """Get the index of this synapse ID"""
        
    def __eq__(self, other):
        if isinstance(other, SynapseId):
            return self.value == other.value
        return False
        
    def __hash__(self):
        return hash(self.value)
        
    def __repr__(self):
        return f"<SynapseId: {self.value}>"
class RegionId:
    """Python wrapper for RegionId"""
    def __init__(self, value: int = 0):
        self.value = value
        
    def index(self) -> int:
        """Get the index of this region ID"""
        
    def __eq__(self, other):
        if isinstance(other, RegionId):
            return self.value == other.value
        return False
        
    def __hash__(self):
        return hash(self.value)
        
    def __repr__(self):
        return f"<RegionId: {self.value}>"
class PopulationId:
    """Python wrapper for PopulationId"""
    def __init__(self, value: int = 0):
        self.value = value
        
    def index(self) -> int:
        """Get the index of this population ID"""
        
    def __eq__(self, other):
        if isinstance(other, PopulationId):
            return self.value == other.value
        return False
        
    def __hash__(self):
        return hash(self.value)
        
    def __repr__(self):
        return f"<PopulationId: {self.value}>"

# Enum-like classes for easy access
class NeuronType:
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"
class SynapseType:
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"
class DevelopmentalStage:
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
class ActionType:
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"
class MotorCommand:
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"
class WorldObjectType:
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
