"""
Agent interface classes for pynlm.

Implements the AgentBrain interface and related classes with full type hints,
Pythonic APIs, and comprehensive error handling.
"""

from typing import Any, Dict, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum

from .exceptions import (
    AgentError, SimulationError, MemoryError, ValidationError,
    raise_agent_error, raise_simulation_error, raise_validation_error
)


class AgentState(Enum):
    """Agent states."""
    IDLE = "idle"
    INITIALIZING = "initializing"
    RUNNING = "running"
    PAUSED = "paused"
    TERMINATED = "terminated"


class LearningFeature(Enum):
    """Learning features."""
    REWARD = "reward"
    STRUCTURAL = "structural"
    DEVELOPMENT = "development"
    CURIOSITY = "curiosity"


@dataclass
class SensoryPercept:
    """Sensory percept data structure."""
    vision_data: Optional[List[float]] = None
    vision_width: int = 0
    vision_height: int = 0
    audio_data: Optional[List[float]] = None
    audio_sample_rate: int = 0
    proprioception_data: Optional[List[float]] = None
    internal_signals: Optional[List[float]] = None
    timestamp: float = 0.0


class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    Provides the high-level interface for agents to interact with the world,
    process sensory input, and produce motor commands.
    
    Examples:
        >>> brain = createBrain()
        >>> agent = AgentBrain(brain)
        >>> agent.initialize(world)
        >>> for step in range(100):
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
    """
    
    def __init__(self, brain):
        """Initialize agent brain with underlying brain.
        
        Args:
            brain: Brain instance to wrap
            
        Raises:
            AgentError: If brain is invalid
        """
        if brain is None:
            raise AgentError("Brain cannot be None", action="initialize")
        
        self._brain = brain
        self._state = AgentState.IDLE
        self._world = None
        
        # Learning features
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        
        # Learning levels
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        
        # Performance tracking
        self._total_steps = 0
        self._total_sensory_processed = 0
        self._total_actions_generated = 0
        self._start_time = 0.0
    
    def initialize(self, world) -> None:
        """Initialize agent with world.
        
        Args:
            world: SimpleWorld instance
            
        Raises:
            AgentError: If initialization fails
        """
        if world is None:
            raise_agent_error("World cannot be None", action="initialize")
        
        try:
            self._world = world
            self._state = AgentState.INITIALIZING
            
            # Store start time for episode tracking
            self._start_time = world.getSimulationTime()
            
            # Default learning features
            self._reward_modulation_enabled = True
            self._structural_plasticity_enabled = True
            self._development_enabled = True
            self._curiosity_enabled = True
            
            self._state = AgentState.RUNNING
            
        except Exception as e:
            self._state = AgentState.ERROR
            raise_agent_error(f"Failed to initialize agent: {str(e)}", action="initialize")
    
    def processSensoryInput(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept data
            
        Raises:
            AgentError: If processing fails
        """
        if self._state != AgentState.RUNNING:
            raise_agent_error(f"Cannot process sensory input in state: {self._state.value}", 
                            action="processSensoryInput")
        
        if percept is None:
            raise_validation_error("Percept cannot be None", field="percept")
        
        try:
            # Convert to C++ SensoryPercept object for brain injection
            cpp_percept = self._convert_to_cpp_percept(percept)
            self._brain.receiveSensoryInput(cpp_percept)
            self._total_sensory_processed += 1
            
        except Exception as e:
            raise_simulation_error(f"Failed to process sensory input: {str(e)}", component="processSensoryInput")
    
    def _convert_to_cpp_percept(self, percept: SensoryPercept):
        """Convert Python SensoryPercept to C++ object."""
        # This would interface with the C++ bindings
        # For now, return a dict that can be converted
        return {
            'vision': percept.vision_data,
            'vision_width': percept.vision_width,
            'vision_height': percept.vision_height,
            'audio': percept.audio_data,
            'audio_sample_rate': percept.audio_sample_rate,
            'proprioception': percept.proprioception_data,
            'internal': percept.internal_signals,
            'timestamp': percept.timestamp,
        }
    
    def decodeMotorCommand(self) -> Any:
        """Decode brain motor activity into motor command.
        
        Returns:
            MotorCommand object
            
        Raises:
            AgentError: If decoding fails
        """
        if self._state != AgentState.RUNNING:
            raise_agent_error(f"Cannot decode motor command in state: {self._state.value}", 
                            action="decodeMotorCommand")
        
        try:
            # Use brain to produce action
            return self._brain.produceAction()
        except Exception as e:
            raise_simulation_error(f"Failed to decode motor command: {str(e)}", component="decodeMotorCommand")
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
            
        Raises:
            AgentError: If neuromodulation fails
        """
        if not self._reward_modulation_enabled:
            return
        
        try:
            # Calculate prediction error
            error = reward - predicted_reward
            self._prediction_error = abs(error)
            
            # Update neuromodulation level
            self._neuromodulation_level = min(1.0, self._neuromodulation_level + error * 0.01)
            
            # Apply to brain (placeholder for actual neuromodulation)
            # brain.applyNeuromodulation(self._neuromodulation_level)
            
        except Exception as e:
            raise_simulation_error(f"Failed to apply reward modulation: {str(e)}", component="applyRewardModulation")
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system.
        
        Args:
            timestep: Time step size
            
        Raises:
            AgentError: If development update fails
        """
        if not self._development_enabled:
            return
        
        try:
            # Update developmental stage through brain
            brain_stage = self._brain.getDevelopmentalStage()
            
            # Calculate curiosity level based on novelty
            self._curiosity_level = min(1.0, self._curiosity_level + 0.01)
            
            # Update novelty level
            self._novelty_level = min(1.0, self._novelty_level + self._prediction_error * 0.1)
            
        except Exception as e:
            raise_simulation_error(f"Failed to update development: {str(e)}", component="updateDevelopment")
    
    # Learning feature control methods
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable or disable reward modulation."""
        self._reward_modulation_enabled = enable
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity."""
        self._structural_plasticity_enabled = enable
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable or disable development."""
        self._development_enabled = enable
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable or disable curiosity."""
        self._curiosity_enabled = enable
    
    # State and status methods
    def getState(self) -> AgentState:
        """Get current agent state."""
        return self._state
    
    def getBrain(self):
        """Get the underlying brain."""
        return self._brain
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        return self._neuromodulation_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        return self._curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        return self._novelty_level
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        return self._prediction_error
    
    def getDevelopmentalStage(self) -> str:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def getTotalSteps(self) -> int:
        """Get total steps executed."""
        return self._total_steps
    
    def getTotalSensoryProcessed(self) -> int:
        """Get total sensory inputs processed."""
        return self._total_sensory_processed
    
    def getTotalActionsGenerated(self) -> int:
        """Get total actions generated."""
        return self._total_actions_generated
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        try:
            self._state = AgentState.IDLE
            self._total_steps = 0
            self._total_sensory_processed = 0
            self._total_actions_generated = 0
            
            # Reset learning features to defaults
            self._reward_modulation_enabled = True
            self._structural_plasticity_enabled = True
            self._development_enabled = True
            self._curiosity_enabled = True
            
            self._neuromodulation_level = 0.0
            self._curiosity_level = 0.0
            self._novelty_level = 0.0
            self._prediction_error = 0.0
            
        except Exception as e:
            raise_agent_error(f"Failed to reset agent: {str(e)}", action="reset")
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<AgentBrain state={self._state.value}, steps={self._total_steps}>"


def createAgentBrain(brain):
    """Create a new AgentBrain instance.
    
    Args:
        brain: Brain instance to wrap
        
    Returns:
        AgentBrain instance
    """
    return AgentBrain(brain)


def enableLearning(agent, features=None):
    """Enable learning features on an agent in a Pythonic way.
    
    Args:
        agent: AgentBrain instance
        features: List of features to enable. If None, enables all.
    """
    if features is None:
        features = ["reward", "structural", "development", "curiosity"]
    
    feature_map = {
        "reward": agent.enableRewardModulation,
        "structural": agent.enableStructuralPlasticity,
        "development": agent.enableDevelopment,
        "curiosity": agent.enableCuriosity,
    }
    
    for feature in features:
        if feature in feature_map:
            feature_map[feature](True)
