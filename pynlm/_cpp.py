# Core C++ bindings for NLM

"""
Core C++ bindings for NLM (Neural Learning Machine).

This module provides the essential C++ bindings that are imported by all other
modules. These bindings form the foundation of the NLM Python interface.
"""

import sys
from typing import Any, List, Optional, Dict, Tuple
import ctypes
import os

# Import the actual C++ bindings
from ._cpp import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, ActionType, DevelopmentalStage,
    FiringState, WorldObjectType,
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID,
    PI, E, FLOAT_EPSILON,
    # Configuration
    Config, ConfigEntry, ConfigSource, ConfigValue,
    # Brain components
    Brain, Neuron, Synapse, Region, Population,
    # Sensory and motor
    SensoryInput, Vision, Audio, InternalSignals,
    Action, ActionResult, SensoryPercept,
    # World
    SimpleWorld, WorldObject,
    # Agent
    AgentBody, AgentBrain,
    # Logging
    Logger, LogLevel, LogEntry, LogStream,
    # Experiment
    Experiment, ExperimentRunner, Phase6IntegratedExperiment,
    # Performance
    PerformanceMetrics, CheckpointSystem, MemoryPool,
    # Core utilities
    SimulationClock, RandomGenerator,
)

# Re-export commonly used constants and types
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'ActionType', 'DevelopmentalStage',
    'FiringState', 'WorldObjectType',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'PI', 'E', 'FLOAT_EPSILON',
    # Configuration
    'Config', 'ConfigEntry', 'ConfigSource', 'ConfigValue',
    # Brain components
    'Brain', 'Neuron', 'Synapse', 'Region', 'Population',
    # Sensory and motor
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'ActionResult', 'SensoryPercept',
    # World
    'SimpleWorld', 'WorldObject',
    # Agent
    'AgentBody', 'AgentBrain',
    # Logging
    'Logger', 'LogLevel', 'LogEntry', 'LogStream',
    # Experiment
    'Experiment', 'ExperimentRunner', 'Phase6IntegratedExperiment',
    # Performance
    'PerformanceMetrics', 'CheckpointSystem', 'MemoryPool',
    # Core utilities
    'SimulationClock', 'RandomGenerator',
]


# Python wrapper classes for better usability
class PythonBrainWrapper:
    """
    Python wrapper for Brain class with enhanced usability.

    This class provides a more Pythonic interface to the Brain class,
    including convenience methods and automatic type conversion.
    """

    def __init__(self, brain: Brain):
        """
        Initialize brain wrapper.

        Args:
            brain: The Brain instance to wrap.
        """
        self._brain = brain
        self._initialized = False

    def initialize(self):
        """Initialize the brain."""
        if self._brain.initialize():
            self._initialized = True
            return True
        return False

    def step(self, step_num: int, time: Optional[float] = None):
        """
        Advance simulation by one step.

        Args:
            step_num: Current simulation step number.
            time: Current simulation time (optional).
        """
        if not self._initialized:
            raise RuntimeError("Brain must be initialized before calling step()")

        if time is None:
            time = step_num * 0.001  # Default timestep

        self._brain.step(step_num, time)

    def injectCurrent(self, neuron_id: NeuronId, current: float):
        """Inject current into a specific neuron."""
        self._brain.injectCurrent(neuron_id, current)

    def injectCurrentToNeurons(self, neuron_type: NeuronType, current: float):
        """Inject current into all neurons of a specific type."""
        self._brain.injectCurrentToNeurons(neuron_type, current)

    def produceAction(self) -> Action:
        """Produce motor action based on neural activity."""
        return self._brain.produceAction()

    def receiveSensoryInput(self, input_data: SensoryInput):
        """Receive sensory input."""
        self._brain.receiveSensoryInput(input_data)

    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
        return self._brain.save(filepath)

    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
        return self._brain.load(filepath)

    def reset(self):
        """Reset brain state."""
        self._brain.reset()
        self._initialized = False

    # Properties for easy access to brain state
    @property
    def total_neurons(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()

    @property
    def total_synapses(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()

    @property
    def firing_neurons(self) -> int:
        """Get count of firing neurons."""
        return self._brain.getFiringNeuronCount()

    @property
    def active_neurons(self) -> int:
        """Get count of active neurons."""
        return self._brain.getActiveNeuronCount()

    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()

    @property
    def total_spikes(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()

    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio."""
        return self._brain.getExcitationInhibitionRatio()

    @property
    def developmental_stage(self) -> DevelopmentalStage:
        """Get developmental stage."""
        return self._brain.getDevelopmentalStage()

    @property
    def working_memory(self):
        """Get working memory instance."""
        return self._brain.getWorkingMemory()

    @property
    def episodic_memory(self):
        """Get episodic memory instance."""
        return self._brain.getEpisodicMemory()

    @property
    def dopamine(self):
        """Get dopamine neuromodulator instance."""
        return self._brain.getDopamine()

    @property
    def curiosity(self):
        """Get curiosity neuromodulator instance."""
        return self._brain.getCuriosity()

    @property
    def novelty(self):
        """Get novelty neuromodulator instance."""
        return self._brain.getNovelty()

    @property
    def prediction_system(self):
        """Get prediction system instance."""
        return self._brain.getPredictionSystem()

    @property
    def development_system(self):
        """Get development system instance."""
        return self._brain.getDevelopmentSystem()

    @property
    def config(self) -> Config:
        """Get brain configuration."""
        return self._brain.getConfig()


class PythonAgentBrainWrapper:
    """
    Python wrapper for AgentBrain class with enhanced usability.
    """

    def __init__(self, agent_brain: AgentBrain):
        """Initialize agent brain wrapper."""
        self._agent_brain = agent_brain

    def initialize(self, world: SimpleWorld):
        """Initialize agent with world."""
        self._agent_brain.initialize(world)

    def processSensoryInput(self, percept: SensoryPercept):
        """Process sensory percept."""
        self._agent_brain.processSensoryInput(percept)

    def decodeMotorCommand(self) -> MotorCommand:
        """Decode motor command from brain activity."""
        return self._agent_brain.decodeMotorCommand()

    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation."""
        self._agent_brain.applyRewardModulation(reward, predicted_reward)

    def updateDevelopment(self, timestep: float):
        """Update development system."""
        self._agent_brain.updateDevelopment(timestep)

    # Properties for easy access to agent state
    @property
    def brain(self) -> Brain:
        """Get underlying brain."""
        return self._agent_brain.getBrain()

    @property
    def developmental_stage(self) -> DevelopmentalStage:
        """Get developmental stage."""
        return self._agent_brain.getDevelopmentalStage()

    @property
    def neuromodulation_level(self) -> float:
        """Get neuromodulation level."""
        return self._agent_brain.getNeuromodulationLevel()

    @property
    def curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent_brain.getCuriosityLevel()

    @property
    def novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent_brain.getNoveltyLevel()

    @property
    def prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent_brain.getPredictionError()


class PythonWorldWrapper:
    """
    Python wrapper for SimpleWorld class with enhanced usability.
    """

    def __init__(self, world: SimpleWorld):
        """Initialize world wrapper."""
        self._world = world

    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions."""
        self._world.configure(width, height, vision_width, vision_height)

    def reset(self):
        """Reset world."""
        self._world.reset()

    def update(self, timestep: float):
        """Update world."""
        self._world.update(timestep)

    def applyMotorCommand(self, cmd: MotorCommand, current_time: float) -> ActionResult:
        """Apply motor command."""
        return self._world.applyMotorCommand(cmd, current_time)

    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept."""
        return self._world.getSensoryPercept()

    def getAgentBody(self) -> AgentBody:
        """Get agent body."""
        return self._world.getAgentBody()

    # Properties
    @property
    def width(self) -> int:
        """Get world width."""
        return self._world.getWidth()

    @property
    def height(self) -> int:
        """Get world height."""
        return self._world.getHeight()

    @property
    def simulation_time(self) -> float:
        """Get simulation time."""
        return self._world.getSimulationTime()


# Pythonic convenience functions

def createBrain(config=None) -> PythonBrainWrapper:
    """
    Create a new brain with enhanced Python interface.

    Args:
        config: Configuration (optional).

    Returns:
        PythonBrainWrapper: Brain with enhanced Python interface.
    """
    if config is None:
        config = createDefaultConfig()

    brain = Brain(config)
    return PythonBrainWrapper(brain)


def createSimpleWorld() -> PythonWorldWrapper:
    """
    Create a simple world with enhanced Python interface.

    Returns:
        PythonWorldWrapper: World with enhanced Python interface.
    """
    world = SimpleWorld()
    return PythonWorldWrapper(world)


def createAgentBrain(brain: Brain) -> PythonAgentBrainWrapper:
    """
    Create an agent brain with enhanced Python interface.

    Args:
        brain: The brain to wrap.

    Returns:
        PythonAgentBrainWrapper: Agent brain with enhanced interface.
    """
    agent_brain = AgentBrain(brain)
    return PythonAgentBrainWrapper(agent_brain)


def createAgentSystem():
    """
    Create a complete agent system with enhanced Python interface.

    Returns:
        tuple: (PythonBrainWrapper, PythonWorldWrapper, PythonAgentBrainWrapper)
    """
    config = createDefaultConfig()
    brain = Brain(config)
    world = SimpleWorld()
    agent_brain = AgentBrain(brain)

    return (
        PythonBrainWrapper(brain),
        PythonWorldWrapper(world),
        PythonAgentBrainWrapper(agent_brain)
    )


def createDefaultConfig() -> Config:
    """Create default configuration."""
    return Config()


# Export commonly used functions
__all__.extend([
    'PythonBrainWrapper', 'PythonAgentBrainWrapper', 'PythonWorldWrapper',
    'createBrain', 'createSimpleWorld', 'createAgentBrain', 'createAgentSystem',
    'createDefaultConfig'
])