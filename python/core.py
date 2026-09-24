"""
Core NLM classes - Python bindings for fundamental neural components.

This module contains the core neural classes that form the foundation of
the NLM artificial brain system.
"""

import sys
from typing import List, Optional, Dict, Any

# Import the C++ bindings
import pynlm_core

__all__ = [
    'Config',
    'Brain', 'Neuron', 'Synapse', 'NeuralRegion', 'Population',
]


class Config:
    """Configuration management for NLM brain simulation.
    
    The configuration system provides template-based value storage with
    multiple sources (file, command line, runtime) and validation.
    
    Example:
        >>> config = Config()
        >>> config.set("neuron_count", 1000)
        >>> config.set("simulation_timestep", 0.001)
        >>> config.loadFromFile("my_config.json")
    """
    
    def __init__(self):
        self._config = pynlm_core.Config()
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file.
            
        Returns:
            bool: True if configuration loaded successfully, False otherwise.
        """
        return self._config.loadFromFile(filepath)
    
    def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
        """Load configuration from command line arguments.
        
        Args:
            argc: Number of arguments.
            argv: List of argument strings.
            
        Returns:
            bool: True if configuration loaded successfully, False otherwise.
        """
        return self._config.loadFromArgs(argc, argv)
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to a JSON file.
        
        Args:
            filepath: Path where to save the configuration.
            
        Returns:
            bool: True if configuration saved successfully, False otherwise.
        """
        return self._config.saveToFile(filepath)
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check.
            
        Returns:
            bool: True if key exists, False otherwise.
        """
        return self._config.has(key)
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List[str]: List of all configuration keys.
        """
        return self._config.getKeys()
    
    def clear(self) -> bool:
        """Clear all configuration entries.
        
        Returns:
            bool: True if configuration cleared successfully, False otherwise.
        """
        return self._config.clear()
    
    def set(self, key: str, value: Any, source: str = "runtime") -> bool:
        """Set a configuration value.
        
        Args:
            key: Configuration key.
            value: Configuration value.
            source: Source of configuration ("file", "command_line", "runtime").
            
        Returns:
            bool: True if value set successfully, False otherwise.
        """
        return self._config.set(key, value, source)
    
    def getOr(self, key: str, default: Any) -> Any:
        """Get configuration value with default fallback.
        
        Args:
            key: Configuration key.
            default: Default value if key not found.
            
        Returns:
            Any: Configuration value or default.
        """
        return self._config.getOr(key, default)
    
    def summary(self) -> str:
        """Get a summary string of the configuration.
        
        Returns:
            str: Summary of configuration.
        """
        return self._config.summary()
    
    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"


class Neuron:
    """Spiking neuron with LIF dynamics.
    
    This class represents an individual neuron in the NLM system,
    implementing Leaky Integrate-and-Fire dynamics with synaptic inputs.
    
    Attributes:
        neuron_id: Unique identifier for the neuron.
        neuron_type: Type of neuron (excitatory, inhibitory, etc.).
    """
    
    def __init__(self, neuron_id: int, neuron_type: str = "Internal"):
        self._neuron = pynlm_core.Neuron(neuron_id, neuron_type)
    
    def injectCurrent(self, current: float) -> bool:
        """Inject current into neuron.
        
        Args:
            current: Current to inject (in mV).
            
        Returns:
            bool: True if current injected successfully.
        """
        return self._neuron.injectCurrent(current)
    
    def stepLIF(self, time: float, dt: float) -> bool:
        """Perform one LIF integration step.
        
        Args:
            time: Current simulation time.
            dt: Time step duration.
            
        Returns:
            bool: True if step completed successfully.
        """
        return self._neuron.stepLIF(time, dt)
    
    def isFiring(self) -> bool:
        """Check if neuron is currently firing.
        
        Returns:
            bool: True if neuron is firing.
        """
        return self._neuron.isFiring()
    
    def getId(self) -> int:
        """Get neuron ID.
        
        Returns:
            int: Neuron ID.
        """
        return self._neuron.getId()
    
    def getType(self) -> str:
        """Get neuron type.
        
        Returns:
            str: Neuron type.
        """
        return self._neuron.getType()
    
    def getState(self) -> Dict[str, Any]:
        """Get current neuron state.
        
        Returns:
            Dict[str, Any]: Neuron state including potential, firing state, etc.
        """
        return self._neuron.getState()
    
    def __repr__(self) -> str:
        return f"<Neuron: id={self.getId()}, type={self.getType()}>"


class Synapse:
    """Synaptic connection between neurons.
    
    This class represents a synaptic connection that transmits signals
    between neurons with specific weight, delay, and type.
    
    Attributes:
        synapse_id: Unique identifier for the synapse.
        source_neuron: Source neuron ID.
        target_neuron: Target neuron ID.
    """
    
    def __init__(self, synapse_id: int, source_neuron: int, target_neuron: int):
        self._synapse = pynlm_core.Synapse(synapse_id, source_neuron, target_neuron)
    
    def getId(self) -> int:
        """Get synapse ID.
        
        Returns:
            int: Synapse ID.
        """
        return self._synapse.getId()
    
    def getSourceNeuron(self) -> int:
        """Get source neuron ID.
        
        Returns:
            int: Source neuron ID.
        """
        return self._synapse.getSourceNeuron()
    
    def getTargetNeuron(self) -> int:
        """Get target neuron ID.
        
        Returns:
            int: Target neuron ID.
        """
        return self._synapse.getTargetNeuron()
    
    def getWeight(self) -> float:
        """Get synaptic weight.
        
        Returns:
            float: Synaptic weight.
        """
        return self._synapse.getWeight()
    
    def setWeight(self, weight: float) -> bool:
        """Set synaptic weight.
        
        Args:
            weight: New synaptic weight.
            
        Returns:
            bool: True if weight set successfully.
        """
        return self._synapse.setWeight(weight)
    
    def getDelay(self) -> int:
        """Get synaptic delay.
        
        Returns:
            int: Synaptic delay (in time steps).
        """
        return self._synapse.getDelay()
    
    def getType(self) -> str:
        """Get synapse type.
        
        Returns:
            str: Synapse type (excitatory, inhibitory, etc.).
        """
        return self._synapse.getType()
    
    def enablePlasticity(self, hebbian: bool, stdp: bool, structural: bool) -> bool:
        """Enable plasticity rules for this synapse.
        
        Args:
            hebbian: Enable Hebbian plasticity.
            stdp: Enable STDP plasticity.
            structural: Enable structural plasticity.
            
        Returns:
            bool: True if plasticity enabled successfully.
        """
        return self._synapse.enablePlasticity(hebbian, stdp, structural)
    
    def getPlasticityFlags(self) -> Dict[str, bool]:
        """Get plasticity flags for this synapse.
        
        Returns:
            Dict[str, bool]: Plasticity flags (hebbian, stdp, structural).
        """
        return self._synapse.getPlasticityFlags()
    
    def __repr__(self) -> str:
        return f"<Synapse: id={self.getId()}, {self.getSourceNeuron()}->{self.getTargetNeuron()}, weight={self.getWeight():.3f}>"


class Brain:
    """Central neural processing unit.
    
    The Brain class is the main neural processor in NLM, coordinating
    all neural subsystems including memory, prediction, cognition, and
    neuromodulation systems.
    
    The brain implements event-driven simulation of spiking neural networks
    with multiple integrated cognitive systems.
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> for i in range(100):
        ...     brain.step(i)
        ... 
        >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
    """
    
    def __init__(self, config: Optional[Config] = None):
        if config is None:
            config = Config()
        self._brain = pynlm_core.Brain(config._config)
    
    def initialize(self) -> bool:
        """Initialize the brain with configuration.
        
        Returns:
            bool: True if initialization successful, False otherwise.
        """
        return self._brain.initialize()
    
    def step(self, step: int) -> None:
        """Perform one simulation step.
        
        Args:
            step: Step number for simulation.
        """
        self._brain.step(step)
    
    def stepWithTime(self, step: int, time: float) -> None:
        """Perform one simulation step with timestamp.
        
        Args:
            step: Step number for simulation.
            time: Current simulation time.
        """
        self._brain.step(step, time)
    
    def receiveSensoryInput(self, input_data: List[float]) -> bool:
        """Receive sensory input from environment.
        
        Args:
            input_data: Sensory input data (e.g., vision, audio).
            
        Returns:
            bool: True if input processed successfully.
        """
        return self._brain.receiveSensoryInput(input_data)
    
    def injectCurrent(self, neuron_id: int, current: float) -> bool:
        """Inject current directly into a specific neuron.
        
        Args:
            neuron_id: Target neuron ID.
            current: Current to inject (in mV).
            
        Returns:
            bool: True if current injected successfully.
        """
        return self._brain.injectCurrent(neuron_id, current)
    
    def injectCurrentToNeurons(self, neuron_type: str, current: float) -> bool:
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons (e.g., "Sensory", "Internal").
            current: Current to inject (in mV).
            
        Returns:
            bool: True if current injected successfully.
        """
        return self._brain.injectCurrentToNeurons(neuron_type, current)
    
    # Statistics methods
    def getTotalNeuronCount(self) -> int:
        """Get total number of neurons in brain.
        
        Returns:
            int: Total neuron count.
        """
        return self._brain.getTotalNeuronCount()
    
    def getTotalSynapseCount(self) -> int:
        """Get total number of synapses in brain.
        
        Returns:
            int: Total synapse count.
        """
        return self._brain.getTotalSynapseCount()
    
    def getActiveNeuronCount(self) -> int:
        """Get number of currently active neurons.
        
        Returns:
            int: Active neuron count.
        """
        return self._brain.getActiveNeuronCount()
    
    def getFiringNeuronCount(self) -> int:
        """Get number of neurons firing in current step.
        
        Returns:
            int: Firing neuron count.
        """
        return self._brain.getFiringNeuronCount()
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count across all neurons.
        
        Returns:
            int: Total spike count.
        """
        return self._brain.getTotalSpikeCount()
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons.
        
        Returns:
            float: Average firing rate (spikes per time unit).
        """
        return self._brain.getAverageFiringRate()
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio.
        
        Returns:
            float: E/I ratio.
        """
        return self._brain.getExcitationInhibitionRatio()
    
    def produceAction(self) -> Dict[str, Any]:
        """Produce motor action based on neural activity.
        
        Returns:
            Dict[str, Any]: Motor action with type and parameters.
        """
        return self._brain.produceAction()
    
    # Memory system accessors
    def getWorkingMemory(self) -> 'WorkingMemory':
        """Get working memory system.
        
        Returns:
            WorkingMemory: Working memory system object.
        """
        return WorkingMemory(self._brain.getWorkingMemory())
    
    def getEpisodicMemory(self) -> 'EpisodicMemory':
        """Get episodic memory system.
        
        Returns:
            EpisodicMemory: Episodic memory system object.
        """
        return EpisodicMemory(self._brain.getEpisodicMemory())
    
    def getAssociativeMemory(self) -> 'AssociativeMemory':
        """Get associative memory system.
        
        Returns:
            AssociativeMemory: Associative memory system object.
        """
        return AssociativeMemory(self._brain.getAssociativeMemory())
    
    # Prediction system accessor
    def getPredictionSystem(self) -> 'PredictionSystem':
        """Get prediction system.
        
        Returns:
            PredictionSystem: Prediction system object.
        """
        return PredictionSystem(self._brain.getPredictionSystem())
    
    # Cognitive system accessors
    def getPlanner(self) -> 'NeuralPlanner':
        """Get neural planner.
        
        Returns:
            NeuralPlanner: Neural planner object.
        """
        return NeuralPlanner(self._brain.getPlanner())
    
    def getConceptFormation(self) -> 'ConceptFormation':
        """Get concept formation system.
        
        Returns:
            ConceptFormation: Concept formation system object.
        """
        return ConceptFormation(self._brain.getConceptFormation())
    
    def getAttention(self) -> 'AttentionalSelection':
        """Get attentional selection system.
        
        Returns:
            AttentionalSelection: Attentional selection system object.
        """
        return AttentionalSelection(self._brain.getAttention())
    
    # Neuromodulation system accessors
    def getDopamine(self) -> 'Dopamine':
        """Get dopamine neuromodulator.
        
        Returns:
            Dopamine: Dopamine neuromodulator object.
        """
        return Dopamine(self._brain.getDopamine())
    
    def getCuriosity(self) -> 'Curiosity':
        """Get curiosity neuromodulator.
        
        Returns:
            Curiosity: Curiosity neuromodulator object.
        """
        return Curiosity(self._brain.getCuriosity())
    
    def getNovelty(self) -> 'Novelty':
        """Get novelty detection system.
        
        Returns:
            Novelty: Novelty detection object.
        """
        return Novelty(self._brain.getNovelty())
    
    def getPredictionErrorSignal(self) -> 'PredictionError':
        """Get prediction error system.
        
        Returns:
            PredictionError: Prediction error system object.
        """
        return PredictionError(self._brain.getPredictionErrorSignal())
    
    # Development system accessor
    def getDevelopmentSystem(self) -> 'DevelopmentSystem':
        """Get development system.
        
        Returns:
            DevelopmentSystem: Development system object.
        """
        return DevelopmentSystem(self._brain.getDevelopmentSystem())
    
    # Configuration accessor
    def getConfig(self) -> Config:
        """Get brain configuration.
        
        Returns:
            Config: Configuration object.
        """
        return Config(self._brain.getConfig())
    
    # Reset and persistence
    def reset(self) -> None:
        """Reset brain to initial state."""
        self._brain.reset()
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: Path where to save brain state.
            
        Returns:
            bool: True if save successful, False otherwise.
        """
        return self._brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: Path to brain state file.
            
        Returns:
            bool: True if load successful, False otherwise.
        """
        return self._brain.load(filepath)
    
    # Logging
    def logStatus(self) -> None:
        """Log brain status to console."""
        self._brain.logStatus()
    
    def __repr__(self) -> str:
        return f"<Brain: neurons={self.getTotalNeuronCount()}, synapses={self.getTotalSynapseCount()}, spikes={self.getTotalSpikeCount()}>"
