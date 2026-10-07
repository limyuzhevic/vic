# Brain implementation for NLM Python API

"""
Central neural simulation brain class

Provides a Pythonic interface to the NLM brain simulation,
with type hints, better error handling, and convenient methods.
"""

from typing import Dict, List, Any, Optional, Union, Iterator
from pathlib import Path
import time

# Import the C++ binding and type definitions
from pynlm import Config
from pynlm import Brain as CppBrain, NeuronId, RegionId, NeuronType
from pynlm.types import SimulationStep, Timestamp, Nanoseconds, MembranePotential

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class BrainInitializationError(NLMError):
    """Raised when brain initialization fails"""
    pass

class BrainStepError(NLMError):
    """Raised when brain step fails"""
    pass

class Brain:
    """
    NLM Brain Simulation
    
    The central neural simulation brain that implements real spiking neural
    computation with event-driven dynamics and integrated memory, prediction,
    cognition, and neuromodulation systems.
    
    The brain operates as a closed-loop system:
    1. World provides sensory input
    2. Brain processes with neural dynamics
    3. Brain produces motor output
    4. World executes action and provides feedback
    5. Feedback updates brain via neuromodulation
    
    Examples:
        >>> config = Config()
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> 
        >>> for step in range(100):
        ...     brain.step(step)  # Think
        ...     action = brain.produce_action()
        ...     print(f"Action: {action}")
    """
    
    def __init__(self, config: Optional[Config] = None):
        """
        Initialize brain with configuration.
        
        Args:
            config: NLM configuration (uses default if None)
            
        Raises:
            BrainInitializationError: If brain cannot be created
            
        Examples:
            >>> brain = Brain()  # Use default config
            >>> brain = Brain(Config())  # Explicit config
        """
        if config is None:
            from pynlm import get_config
            config = get_config()
        
        try:
            self._cpp_brain = CppBrain(config._cpp_config)
            self._initialized = False
        except Exception as e:
            raise BrainInitializationError(f"Failed to create brain: {e}")
    
    def initialize(self) -> bool:
        """
        Initialize brain with configuration.
        
        Always call this before using the brain!
        
        Returns:
            True if initialization succeeded, False otherwise
            
        Raises:
            BrainInitializationError: If initialization fails
            
        Examples:
            >>> brain = Brain()
            >>> if brain.initialize():
            ...     print("Brain initialized successfully")
            else:
            ...     print("Brain initialization failed")
        """
        try:
            result = self._cpp_brain.initialize()
            self._initialized = result
            return result
        except Exception as e:
            raise BrainInitializationError(f"Failed to initialize brain: {e}")
    
    def step(self, step: SimulationStep, time: Optional[Timestamp] = None) -> None:
        """
        Perform a simulation step.
        
        Args:
            step: Current simulation step number
            time: Optional simulation time (defaults to step number)
            
        Raises:
            BrainStepError: If step fails
            RuntimeError: If brain is not initialized
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> for i in range(100):
            ...     brain.step(i)  # Think for step i
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized. Call initialize() first.")
        
        try:
            if time is None:
                self._cpp_brain.step(step)
            else:
                self._cpp_brain.step(step, time)
        except Exception as e:
            raise BrainStepError(f"Failed to step brain at step {step}: {e}")
    
    def step_with_time(self, step: SimulationStep, time: Timestamp) -> None:
        """
        Perform a simulation step with explicit time.
        
        Args:
            step: Current simulation step number
            time: Current simulation time in seconds
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> brain.step_with_time(10, 0.1)  # Step 10 at t=0.1s
        """
        self.step(step, time)
    
    def step_with_timestamp(self, step: SimulationStep, timestamp: Timestamp) -> None:
        """
        Perform a simulation step with timestamp (alias for step_with_time).
        
        Args:
            step: Current simulation step number
            timestamp: Current simulation timestamp
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> brain.step_with_timestamp(10, 0.1)
        """
        self.step(step, timestamp)
    
    def process_sensory_input(self, sensory_input: Any) -> None:
        """
        Inject sensory input into the brain.
        
        Args:
            sensory_input: Sensory input object (Vision, Audio, etc.)
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> vision = Vision(width=8, height=8, channels=3)
            >>> brain.process_sensory_input(vision)
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized. Call initialize() first.")
        
        try:
            self._cpp_brain.receiveSensoryInput(sensory_input)
        except Exception as e:
            raise BrainStepError(f"Failed to process sensory input: {e}")
    
    def inject_current(self, neuron_id: NeuronId, current: MembranePotential) -> None:
        """
        Inject current directly into a specific neuron.
        
        Args:
            neuron_id: ID of neuron to inject current into
            current: Current to inject (in microvolts)
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> brain.inject_current(neuron_id, 1000.0)  # 1 mV current
        """
        try:
            self._cpp_brain.injectCurrent(neuron_id, current)
        except Exception as e:
            raise BrainStepError(f"Failed to inject current: {e}")
    
    def inject_current_to_type(self, neuron_type: NeuronType, current: MembranePotential) -> None:
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to inject current into
            current: Current to inject (in microvolts)
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> brain.inject_current_to_type(NeuronType.Excitatory, 500.0)
        """
        try:
            self._cpp_brain.injectCurrentToNeurons(neuron_type, current)
        except Exception as e:
            raise BrainStepError(f"Failed to inject current to type: {e}")
    
    def produce_action(self) -> Any:
        """
        Produce motor action based on neural activity.
        
        Returns:
            Motor action object
            
        Raises:
            BrainStepError: If action production fails
            RuntimeError: If brain is not initialized
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> action = brain.produce_action()
            >>> print(f"Action type: {action.getType()}")
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_brain.produceAction()
        except Exception as e:
            raise BrainStepError(f"Failed to produce action: {e}")
    
    def reset(self) -> None:
        """
        Reset brain state.
        
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> # ... run simulation ...
            >>> brain.reset()  # Reset to initial state
        """
        try:
            self._cpp_brain.reset()
            self._initialized = False
        except Exception as e:
            raise BrainStepError(f"Failed to reset brain: {e}")
    
    def save(self, filepath: str) -> bool:
        """
        Save brain state to file (checkpointing).
        
        Args:
            filepath: Path to save brain state
            
        Returns:
            True if save succeeded, False otherwise
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> if brain.save("checkpoint.json"):
            ...     print("Brain saved successfully")
        """
        try:
            return self._cpp_brain.save(filepath)
        except Exception as e:
            raise BrainStepError(f"Failed to save brain: {e}")
    
    def load(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
            
        Returns:
            True if load succeeded, False otherwise
            
        Examples:
            >>> brain = Brain()
            >>> if brain.load("checkpoint.json"):
            ...     print("Brain loaded successfully")
            ...     brain.initialize()  # Need to reinitialize
        """
        try:
            result = self._cpp_brain.load(filepath)
            if result:
                self._initialized = True
            return result
        except Exception as e:
            raise BrainStepError(f"Failed to load brain: {e}")
    
    def add_region(self, name: str = "") -> RegionId:
        """
        Add a new neural region.
        
        Args:
            name: Optional region name
            
        Returns:
            ID of newly created region
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> region_id = brain.add_region("Visual Cortex")
            >>> print(f"Created region with ID: {region_id}")
        """
        try:
            return self._cpp_brain.addRegion(name)
        except Exception as e:
            raise BrainStepError(f"Failed to add region: {e}")
    
    def get_region(self, region_id: RegionId) -> Any:
        """
        Get neural region by ID.
        
        Args:
            region_id: ID of region to retrieve
            
        Returns:
            Neural region object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> region_id = brain.add_region("Test Region")
            >>> region = brain.get_region(region_id)
        """
        try:
            return self._cpp_brain.getRegion(region_id)
        except Exception as e:
            raise BrainStepError(f"Failed to get region: {e}")
    
    def get_region_count(self) -> int:
        """
        Get number of regions in brain.
        
        Returns:
            Number of regions
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> regions = brain.get_region_count()
            >>> print(f"Brain has {regions} regions")
        """
        return self._cpp_brain.getRegionCount()
    
    def get_region_ids(self) -> List[RegionId]:
        """
        Get all region IDs.
        
        Returns:
            List of all region IDs
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> region_ids = brain.get_region_ids()
        """
        return self._cpp_brain.getRegionIds()
    
    def get_total_neuron_count(self) -> int:
        """
        Get total neuron count across all regions.
        
        Returns:
            Total number of neurons
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> neurons = brain.get_total_neuron_count()
            >>> print(f"Brain has {neurons} neurons total")
        """
        return self._cpp_brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """
        Get total synapse count across all regions.
        
        Returns:
            Total number of synapses
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> synapses = brain.get_total_synapse_count()
            >>> print(f"Brain has {synapses} synapses total")
        """
        return self._cpp_brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        """
        Get count of active neurons.
        
        Returns:
            Number of active neurons
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> active = brain.get_active_neuron_count()
        """
        return self._cpp_brain.getActiveNeuronCount()
    
    def get_firing_neuron_count(self) -> int:
        """
        Get count of currently firing neurons.
        
        Returns:
            Number of firing neurons
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> firing = brain.get_firing_neuron_count()
            >>> print(f"{firing} neurons are firing")
        """
        return self._cpp_brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """
        Get average firing rate across all neurons.
        
        Returns:
            Average firing rate (spikes per second)
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> rate = brain.get_average_firing_rate()
            >>> print(f"Average firing rate: {rate:.2f} Hz")
        """
        return self._cpp_brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """
        Get excitation/inhibition balance ratio.
        
        Returns:
            EI ratio (higher means more excitation)
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> ei_ratio = brain.get_excitation_inhibition_ratio()
            >>> print(f"EI ratio: {ei_ratio:.3f}")
        """
        return self._cpp_brain.getExcitationInhibitionRatio()
    
    def get_total_spike_count(self) -> int:
        """
        Get total spike count.
        
        Returns:
            Total number of spikes emitted
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> spikes = brain.get_total_spike_count()
            >>> print(f"Brain has emitted {spikes} spikes total")
        """
        return self._cpp_brain.getTotalSpikeCount()
    
    def get_developmental_stage(self):
        """
        Get current developmental stage.
        
        Returns:
            Developmental stage
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> stage = brain.get_developmental_stage()
            >>> print(f"Developmental stage: {stage}")
        """
        return self._cpp_brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage: Any) -> None:
        """
        Set developmental stage.
        
        Args:
            stage: Developmental stage to set
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> brain.set_developmental_stage(DevelopmentalStage.Adult)
        """
        self._cpp_brain.setDevelopmentalStage(stage)
    
    def get_spike_system(self) -> Any:
        """
        Get spike system pointer.
        
        Returns:
            Spike system object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> spike_system = brain.get_spike_system()
        """
        return self._cpp_brain.getSpikeSystem()
    
    def get_stdp(self) -> Any:
        """
        Get STDP plasticity rule pointer.
        
        Returns:
            STDP plasticity rule object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> stdp = brain.get_stdp()
        """
        return self._cpp_brain.getSTDP()
    
    def get_hebbian(self) -> Any:
        """
        Get Hebbian plasticity rule pointer.
        
        Returns:
            Hebbian plasticity rule object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> hebbian = brain.get_hebbian()
        """
        return self._cpp_brain.getHebbian()
    
    def get_structural_plasticity(self) -> Any:
        """
        Get structural plasticity rule pointer.
        
        Returns:
            Structural plasticity rule object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> structural_plasticity = brain.get_structural_plasticity()
        """
        return self._cpp_brain.getStructuralPlasticity()
    
    def get_working_memory(self) -> Any:
        """
        Get working memory pointer.
        
        Returns:
            Working memory object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> working_memory = brain.get_working_memory()
        """
        return self._cpp_brain.getWorkingMemory()
    
    def get_episodic_memory(self) -> Any:
        """
        Get episodic memory pointer.
        
        Returns:
            Episodic memory object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> episodic_memory = brain.get_episodic_memory()
        """
        return self._cpp_brain.getEpisodicMemory()
    
    def get_associative_memory(self) -> Any:
        """
        Get associative memory pointer.
        
        Returns:
            Associative memory object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> associative_memory = brain.get_associative_memory()
        """
        return self._cpp_brain.getAssociativeMemory()
    
    def get_prediction_system(self) -> Any:
        """
        Get prediction system pointer.
        
        Returns:
            Prediction system object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> prediction_system = brain.get_prediction_system()
        """
        return self._cpp_brain.getPredictionSystem()
    
    def get_planner(self) -> Any:
        """
        Get neural planner pointer.
        
        Returns:
            Neural planner object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> planner = brain.get_planner()
        """
        return self._cpp_brain.getPlanner()
    
    def get_concept_formation(self) -> Any:
        """
        Get concept formation pointer.
        
        Returns:
            Concept formation object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> concept_formation = brain.get_concept_formation()
        """
        return self._cpp_brain.getConceptFormation()
    
    def get_attention(self) -> Any:
        """
        Get attentional selection pointer.
        
        Returns:
            Attentional selection object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> attention = brain.get_attention()
        """
        return self._cpp_brain.getAttention()
    
    def get_development_system(self) -> Any:
        """
        Get development system pointer.
        
        Returns:
            Development system object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> development_system = brain.get_development_system()
        """
        return self._cpp_brain.getDevelopmentSystem()
    
    def get_dopamine(self) -> Any:
        """
        Get dopamine neuromodulator pointer.
        
        Returns:
            Dopamine neuromodulator object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> dopamine = brain.get_dopamine()
        """
        return self._cpp_brain.getDopamine()
    
    def get_curiosity(self) -> Any:
        """
        Get curiosity neuromodulator pointer.
        
        Returns:
            Curiosity neuromodulator object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> curiosity = brain.get_curiosity()
        """
        return self._cpp_brain.getCuriosity()
    
    def get_novelty(self) -> Any:
        """
        Get novelty neuromodulator pointer.
        
        Returns:
            Novelty neuromodulator object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> novelty = brain.get_novelty()
        """
        return self._cpp_brain.getNovelty()
    
    def get_prediction_error(self) -> Any:
        """
        Get prediction error neuromodulator pointer.
        
        Returns:
            Prediction error neuromodulator object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> prediction_error = brain.get_prediction_error()
        """
        return self._cpp_brain.getPredictionErrorSignal()
    
    def get_random_generator(self) -> Any:
        """
        Get random generator pointer.
        
        Returns:
            Random generator object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> random_gen = brain.get_random_generator()
        """
        return self._cpp_brain.getRandomGenerator()
    
    def get_config(self) -> Config:
        """
        Get brain configuration.
        
        Returns:
            Configuration object
            
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> config = brain.get_config()
            >>> print(config["brain.regionCount"])
        """
        return config
    
    def log_status(self) -> None:
        """
        Log brain status.
        
        Examples:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> brain.log_status()  # Print status to log
        """
        self._cpp_brain.logStatus()
    
    def __repr__(self) -> str:
        """String representation of brain"""
        if self._initialized:
            return f"<Brain initialized with {self.get_total_neuron_count()} neurons>"
        else:
            return "<Brain not initialized>"
    
    def __str__(self) -> str:
        """String representation of brain"""
        return self.__repr__()

# Convenience functions for brain creation
def create_brain(config: Optional[Config] = None) -> Brain:
    """
    Create a new brain with configuration.
    
    Args:
        config: NLM configuration (uses default if None)
        
    Returns:
        New Brain instance
        
    Examples:
        >>> brain = create_brain()
        >>> brain = create_brain(Config())
    """
    return Brain(config)

# Constants for invalid IDs
INVALID_NEURON_ID = -1
INVALID_SYNAPSE_ID = -1
INVALID_REGION_ID = -1
INVALID_POPULATION_ID = -1