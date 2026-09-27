"""
Core NLM classes and interfaces.

This module contains the main classes for neural computation, including
Brain, Config, AgentBrain, and SimpleWorld classes.
"""

import numpy as np
from typing import Optional, List, Dict, Any, Callable
from enum import Enum

# Import from C++ bindings
import pynlm._bindings as bindings
from pynlm.types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType
)
from pynlm.sensory import Vision, Audio, InternalSignals, SensoryInput
from pynlm.motor import Action
from pynlm.environment import WorldObject
class Config:
    """Configuration for NLM simulation."""
    
    def __init__(self):
        self._config = bindings.create_default_config()
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        return self._config.loadFromFile(filepath)
    
    def load_from_args(self, argc: int, argv: List[str]) -> bool:
        """Load configuration from command line arguments."""
        return self._config.loadFromArgs(argc, argv)
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
        return self._config.saveToFile(filepath)
    
    def has(self, key: str) -> bool:
        """Check if configuration has key."""
        return self._config.has(key)
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value by key with default."""
        return self._config.get(key) if self.has(key) else default
    
    def set(self, key: str, value: Any, source: str = "user") -> None:
        """Set configuration value."""
        self._config.set(key, value, source)
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return self._config.getKeys()
    
    def get_summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()
    
    def __repr__(self) -> str:
        return f"<Config: {self.get_summary()}>"
class Brain:
    """Central neural brain simulation.
    
    The Brain class implements the core neural computation engine with
    spiking neurons, synaptic connections, plasticity mechanisms, and
    memory systems.
    """
    
    def __init__(self, config: Optional[Config] = None):
        if config is None:
            config = Config()
        
        self._brain = bindings.create_brain(config._config)
        self._event_callbacks: Dict[str, List[Callable]] = {}
        self._event_lock = threading.RLock()
    
    def initialize(self) -> bool:
        """Initialize the brain with current configuration."""
        result = self._brain.initialize()
        if result:
            self._event_callbacks['brain_initialized'] = []
        return result
    
    def step(self, current_step: int, current_time: float) -> None:
        """Perform a simulation step."""
        self._brain.step(current_step, current_time)
        
        # Emit step event
        self._emit_event('simulation_step', {
            'step': current_step,
            'time': current_time,
            'active_neurons': self._brain.getActiveNeuronCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'avg_firing_rate': self._brain.getAverageFiringRate()
        })
    
    def receive_sensory_input(self, input_data: SensoryInput) -> None:
        """Inject sensory input into the brain."""
        self._brain.receiveSensoryInput(input_data)
    
    def inject_current(self, neuron_id: NeuronId, current: float) -> None:
        """Inject current into a specific neuron."""
        self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float) -> None:
        """Inject current into all neurons of specified type."""
        self._brain.injectCurrentToNeurons(neuron_type, current)
    
    def produce_action(self) -> Action:
        """Produce motor action based on neural activity."""
        return self._brain.produceAction()
    
    def reset(self) -> None:
        """Reset brain state."""
        self._brain.reset()
        self._emit_event('brain_reset')
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
        return self._brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
        return self._brain.load(filepath)
    
    def add_region(self, name: str = "") -> RegionId:
        """Add a new neural region."""
        return self._brain.addRegion(name)
    
    def get_region(self, region_id: RegionId) -> Optional['NeuralRegion']:
        """Get region by ID."""
        return self._brain.getRegion(region_id)
    
    def get_region_count(self) -> int:
        """Get number of regions."""
        return self._brain.getRegionCount()
    
    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs."""
        return self._brain.getRegionIds()
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        """Get active neuron count."""
        return self._brain.getActiveNeuronCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get firing neuron count."""
        return self._brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage: DevelopmentalStage) -> None:
        """Set developmental stage."""
        self._brain.setDevelopmentalStage(stage)
    
    def get_config(self) -> Config:
        """Get brain configuration."""
        return Config()  # Note: This is simplified
    
    def log_status(self) -> None:
        """Log brain status information."""
        self._brain.logStatus()
    
    # Memory systems
    def get_working_memory(self) -> Optional['NeuralWorkingMemory']:
        """Get working memory system."""
        return self._brain.getWorkingMemory()
    
    def get_episodic_memory(self) -> Optional['NeuralEpisodicMemory']:
        """Get episodic memory system."""
        return self._brain.getEpisodicMemory()
    
    def get_semantic_memory(self) -> Optional['SemanticMemory']:
        """Get semantic memory system."""
        return self._brain.getSemanticMemory()
    
    def get_procedural_memory(self) -> Optional['ProceduralMemory']:
        """Get procedural memory system."""
        return self._brain.getProceduralMemory()
    
    def get_associative_memory(self) -> Optional['AssociativeMemory']:
        """Get associative memory system."""
        return self._brain.getAssociativeMemory()
    
    # Neuromodulation systems
    def get_dopamine(self) -> Optional['Neuromodulator']:
        """Get dopamine system."""
        return self._brain.getDopamine()
    
    def get_curiosity(self) -> Optional['Neuromodulator']:
        """Get curiosity system."""
        return self._brain.getCuriosity()
    
    def get_novelty(self) -> Optional['Neuromodulator']:
        """Get novelty detection system."""
        return self._brain.getNovelty()
    
    def get_prediction_error(self) -> Optional['PredictionError']:
        """Get prediction error system."""
        return self._brain.getPredictionError()
    
    # Cognition systems
    def get_planner(self) -> Optional['NeuralPlanner']:
        """Get neural planner."""
        return self._brain.getPlanner()
    
    def get_concept_formation(self) -> Optional['ConceptFormation']:
        """Get concept formation system."""
        return self._brain.getConceptFormation()
    
    def get_attention(self) -> Optional['AttentionalSelection']:
        """Get attention system."""
        return self._brain.getAttention()
    
    # Prediction systems
    def get_prediction_system(self) -> Optional['PredictionSystem']:
        """Get prediction system."""
        return self._brain.getPredictionSystem()
    
    def get_neural_prediction(self) -> Optional['NeuralPrediction']:
        """Get neural prediction system."""
        return self._brain.getNeuralPrediction()
    
    # Development system
    def get_development_system(self) -> Optional['DevelopmentSystem']:
        """Get development system."""
        return self._brain.getDevelopmentSystem()
    
    # Spike system
    def get_spike_system(self) -> Optional['SpikeSystem']:
        """Get spike system."""
        return self._brain.getSpikeSystem()
    
    # Plasticity system
    def get_stdp(self) -> Optional['STDP']:
        """Get STDP plasticity rule."""
        return self._brain.getSTDP()
    
    def get_hebbian(self) -> Optional['Hebbian']:
        """Get Hebbian plasticity rule."""
        return self._brain.getHebbian()
    
    # Advanced simulation controls
    def enable_plasticity(self, hebbian: bool = True, stdp: bool = True, structural: bool = False) -> None:
        """Enable plasticity rules."""
        # Implementation would go here
        pass
    
    def enable_reward_modulation(self, enable: bool = True) -> None:
        """Enable reward modulation."""
        self._brain.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool = True) -> None:
        """Enable structural plasticity."""
        self._brain.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool = True) -> None:
        """Enable development system."""
        self._brain.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool = True) -> None:
        """Enable curiosity system."""
        self._brain.enableCuriosity(enable)
    
    # Status and monitoring
    def is_plasticity_enabled(self) -> bool:
        """Check if plasticity is enabled."""
        return self._brain.isPlasticityEnabled()
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._brain.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._brain.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._brain.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._brain.isCuriosityEnabled()
    
    # Event system integration
    def on(self, event_type: str, callback: Callable) -> None:
        """Register callback for event type."""
        with self._event_lock:
            if event_type not in self._event_callbacks:
                self._event_callbacks[event_type] = []
            self._event_callbacks[event_type].append(callback)
    
    def off(self, event_type: str, callback: Callable) -> None:
        """Remove callback for event type."""
        with self._event_lock:
            if event_type in self._event_callbacks:
                try:
                    self._event_callbacks[event_type].remove(callback)
                except ValueError:
                    pass
    
    def _emit_event(self, event_type: str, data: Any = None) -> None:
        """Emit event to registered callbacks."""
        with self._event_lock:
            callbacks = self._event_callbacks.get(event_type, []).copy()
        
        for callback in callbacks:
            try:
                callback(data)
            except Exception as e:
                print(f"Error in event callback: {e}")
    
    def __repr__(self) -> str:
        return f"<Brain: {self.get_total_neuron_count()} neurons, {self.get_total_synapse_count()} synapses>"

# Additional core classes
class NeuralRegion:
    """Neural region containing populations of neurons."""
    
    def __init__(self, region_id: RegionId, name: str = ""):
        self.region_id = region_id
        self.name = name
        self._region = bindings.get_region(region_id)
    
    def get_id(self) -> RegionId:
        """Get region ID."""
        return self.region_id
    
    def get_name(self) -> str:
        """Get region name."""
        return self.name

# Threading import for event system
import threading