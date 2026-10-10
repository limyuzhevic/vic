"""
Pythonic interface for NLM agent system.

This module provides interfaces for agent entities that interact with the environment
through sensory perception and motor actions. The agent system bridges the neural
brain with the external world, handling sensory input processing and motor output
coordination.
"""

from typing import Optional, List, Any, Union
from dataclasses import dataclass
import numpy as np

# Import the C++ extension module directly
import pynlm as _c_extension
from .core import Config, ConfigError

if _c_extension is None:
    raise ImportError(
        "NLM C++ extension not found. Please build the extension first:\n"
        "mkdir build && cd build && cmake .. && make"
    )
class AgentError(Exception):
    """Raised when agent operations fail."""
    pass
@dataclass
class PerceptData:
    """Container for sensory percept data.
    
    Attributes:
        vision: Vision sensory data (width x height x channels)
        audio: Audio sensory data (sample_rate x num_samples)
        touch: Touch sensory data
        internal: Internal signals
        proprioception: Proprioceptive data
        timestamp: Sensor timestamp
    """
    vision: Optional[Any] = None
    audio: Optional[Any] = None
    touch: Optional[Any] = None
    internal: Optional[Any] = None
    proprioception: Optional[Any] = None
    timestamp: Optional[float] = None
@dataclass
class ActionResultData:
    """Container for action execution results.
    
    Attributes:
        reward: Reward value from action execution
        success: Whether the action was successful
        message: Optional descriptive message
    """
    reward: float
    success: bool
    message: str = ""
class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    The AgentBrain class provides a higher-level interface for interacting with
    the neural brain system in the context of an agent operating in an environment.
    It handles the flow of sensory information to the brain and motor commands
    from the brain to the world.
    
    Examples:
        >>> from pynlm import Brain, Config, AgentBrain, SimpleWorld
        >>> config = Config(neuron_count=100)
        >>> brain = Brain(config)
        >>> world = SimpleWorld(64, 64, 32, 32)
        >>> agent_brain = AgentBrain(brain)
        >>> agent_brain.initialize(world)
        >>> 
        >>> # Process sensory input and produce actions
        >>> percept = world.getSensoryPercept()
        >>> agent_brain.processSensoryInput(percept)
        >>> action = agent_brain.decodeMotorCommand()
        >>> 
        >>> # Apply action to world
        >>> world.applyMotorCommand(action, world.getSimulationTime())
    """
    
    def __init__(self, brain: Any) -> None:
        """Create an agent brain interface.
        
        Args:
            brain: The underlying Brain object to interface with.
            
        Examples:
            >>> agent_brain = AgentBrain(brain)
        """
        self._brain = brain
        self._world = None
        self._is_initialized = False
    
    def initialize(self, world: Any) -> 'AgentBrain':
        """Initialize the agent brain with world interface.
        
        Args:
            world: The world object to interact with.
            
        Returns:
            Self for method chaining.
            
        Raises:
            AgentError: If initialization fails.
            
        Examples:
            >>> agent_brain.initialize(world)
        """
        try:
            self._world = world
            self._brain.initialize()
            self._is_initialized = True
            return self
        except Exception as e:
            raise AgentError(f"Failed to initialize agent brain: {e}")
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            Size of expected sensory input vector.
            
        Examples:
            >>> size = agent_brain.get_sensory_input_size()
        """
        return self._brain.getSensoryInputSize()
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size.
        
        Returns:
            Size of expected motor output vector.
            
        Examples:
            >>> size = agent_brain.get_motor_output_size()
        """
        return self._brain.getMotorOutputSize()
    
    def process_sensory_input(self, percept: Any) -> None:
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept object containing all sensory modalities.
            
        Raises:
            AgentError: If sensory processing fails.
            
        Examples:
            >>> agent_brain.processSensoryInput(percept)
        """
        if not self._is_initialized:
            raise AgentError("Agent brain must be initialized before processing input")
        
        try:
            self._brain.processSensoryInput(percept)
        except Exception as e:
            raise AgentError(f"Failed to process sensory input: {e}")
    
    def decode_motor_command(self) -> Any:
        """Decode brain motor activity into motor command.
        
        Returns:
            Motor command object based on current brain activity.
            
        Raises:
            AgentError: If motor command decoding fails.
            
        Examples:
            >>> action = agent_brain.decodeMotorCommand()
        """
        try:
            return self._brain.decodeMotorCommand()
        except Exception as e:
            raise AgentError(f"Failed to decode motor command: {e}")
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received from action.
            predicted_reward: Expected reward predicted by brain.
            
        Raises:
            AgentError: If reward modulation fails.
            
        Examples:
            >>> agent_brain.apply_reward_modulation(1.0, 0.8)
        """
        try:
            self._brain.applyRewardModulation(reward, predicted_reward)
        except Exception as e:
            raise AgentError(f"Failed to apply reward modulation: {e}")
    
    def update_development(self, timestep: float) -> None:
        """Update development system.
        
        Args:
            timestep: Time step for development updates.
            
        Raises:
            AgentError: If development update fails.
            
        Examples:
            >>> agent_brain.updateDevelopment(0.1)
        """
        try:
            self._brain.updateDevelopment(timestep)
        except Exception as e:
            raise AgentError(f"Failed to update development: {e}")
    
    def get_developmental_stage(self) -> Any:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage enum value.
            
        Examples:
            >>> stage = agent_brain.get_developmental_stage()
        """
        return self._brain.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level (0.0 to 1.0).
            
        Examples:
            >>> level = agent_brain.get_neuromodulation_level()
        """
        return self._brain.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get current curiosity level.
        
        Returns:
            Current curiosity level (0.0 to 1.0).
            
        Examples:
            >>> level = agent_brain.get_curiosity_level()
        """
        return self._brain.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get current novelty detection level.
        
        Returns:
            Current novelty level (0.0 to 1.0).
            
        Examples:
            >>> level = agent_brain.get_novelty_level()
        """
        return self._brain.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get current prediction error.
        
        Returns:
            Current prediction error value.
            
        Examples:
            >>> error = agent_brain.get_prediction_error()
        """
        return self._brain.getPredictionError()
    
    def reset(self) -> 'AgentBrain':
        """Reset agent for new episode.
        
        Returns:
            Self for method chaining.
            
        Examples:
            >>> agent_brain.reset()
        """
        try:
            self._brain.reset()
            self._is_initialized = False
            return self
        except Exception as e:
            raise AgentError(f"Failed to reset agent brain: {e}")
    
    def get_brain(self) -> Any:
        """Get the underlying brain object.
        
        Returns:
            The Brain object this agent brain interfaces with.
            
        Examples:
            >>> brain = agent_brain.get_brain()
        """
        return self._brain
    
    # Enable/Disable features
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation.
        
        Args:
            enable: True to enable, False to disable.
            
        Examples:
            >>> agent_brain.enable_reward_modulation(True)
        """
        self._brain.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity.
        
        Args:
            enable: True to enable, False to disable.
            
        Examples:
            >>> agent_brain.enable_structural_plasticity(True)
        """
        self._brain.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool) -> None:
        """Enable or disable development system.
        
        Args:
            enable: True to enable, False to disable.
            
        Examples:
            >>> agent_brain.enable_development(True)
        """
        self._brain.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity system.
        
        Args:
            enable: True to enable, False to disable.
            
        Examples:
            >>> agent_brain.enable_curiosity(True)
        """
        self._brain.enableCuriosity(enable)
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled, False otherwise.
            
        Examples:
            >>> enabled = agent_brain.is_reward_modulation_enabled()
        """
        return self._brain.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled, False otherwise.
            
        Examples:
            >>> enabled = agent_brain.is_structural_plasticity_enabled()
        """
        return self._brain.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development system is enabled.
        
        Returns:
            True if development system is enabled, False otherwise.
            
        Examples:
            >>> enabled = agent_brain.is_development_enabled()
        """
        return self._brain.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity system is enabled.
        
        Returns:
            True if curiosity system is enabled, False otherwise.
            
        Examples:
            >>> enabled = agent_brain.is_curiosity_enabled()
        """
        return self._brain.isCuriosityEnabled()
    
    def is_initialized(self) -> bool:
        """Check if agent brain is initialized.
        
        Returns:
            True if agent brain is initialized, False otherwise.
            
        Examples:
            >>> initialized = agent_brain.is_initialized()
        """
        return self._is_initialized
    
    def __repr__(self) -> str:
        """String representation of agent brain."""
        status = "initialized" if self._is_initialized else "not initialized"
        return f"<AgentBrain: {status}>"
    
    def __str__(self) -> str:
        """String representation of agent brain."""
        return f"AgentBrain(brain={self._brain})"
class AgentBody:
    """Agent body state and properties.
    
    The AgentBody class represents the physical properties and state of an agent
    in the simulated environment, including position, orientation, velocity,
    energy, health, and age.
    
    Examples:
        >>> from pynlm import AgentBody
        >>> body = AgentBody()
        >>> body.x = 32.0
        >>> body.y = 32.0
        >>> body.orientation = 0.0
        >>> body.energy = 100.0
        >>> body.health = 1.0
        >>> body.reset()
    """
    
    def __init__(self) -> None:
        """Create an empty agent body state."""
        self._body = _c_extension.AgentBody()
    
    @property
    def x(self) -> float:
        """Get x position."""
        return self._body.x
    
    @x.setter
    def x(self, value: float) -> None:
        """Set x position."""
        self._body.x = value
    
    @property
    def y(self) -> float:
        """Get y position."""
        return self._body.y
    
    @y.setter
    def y(self, value: float) -> None:
        """Set y position."""
        self._body.y = value
    
    @property
    def orientation(self) -> float:
        """Get orientation (radians)."""
        return self._body.orientation
    
    @orientation.setter
    def orientation(self, value: float) -> None:
        """Set orientation (radians)."""
        self._body.orientation = value
    
    @property
    def velocity_x(self) -> float:
        """Get x velocity."""
        return self._body.velocityX
    
    @velocity_x.setter
    def velocity_x(self, value: float) -> None:
        """Set x velocity."""
        self._body.velocityX = value
    
    @property
    def velocity_y(self) -> float:
        """Get y velocity."""
        return self._body.velocityY
    
    @velocity_y.setter
    def velocity_y(self, value: float) -> None:
        """Set y velocity."""
        self._body.velocityY = value
    
    @property
    def angular_velocity(self) -> float:
        """Get angular velocity."""
        return self._body.angularVelocity
    
    @angular_velocity.setter
    def angular_velocity(self, value: float) -> None:
        """Set angular velocity."""
        self._body.angularVelocity = value
    
    @property
    def energy(self) -> float:
        """Get energy level (0-100)."""
        return self._body.energy
    
    @energy.setter
    def energy(self, value: float) -> None:
        """Set energy level (0-100)."""
        self._body.energy = value
    
    @property
    def health(self) -> float:
        """Get health level (0-1)."""
        return self._body.health
    
    @health.setter
    def health(self, value: float) -> None:
        """Set health level (0-1)."""
        self._body.health = value
    
    @property
    def age(self) -> float:
        """Get age in simulation steps."""
        return self._body.age
    
    @age.setter
    def age(self, value: float) -> None:
        """Set age in simulation steps."""
        self._body.age = value
    
    @property
    def is_moving(self) -> bool:
        """Get whether agent is currently moving."""
        return self._body.isMoving
    
    @is_moving.setter
    def is_moving(self, value: bool) -> None:
        """Set whether agent is currently moving."""
        self._body.isMoving = value
    
    @property
    def is_turning(self) -> bool:
        """Get whether agent is currently turning."""
        return self._body.isTurning
    
    @is_turning.setter
    def is_turning(self, value: bool) -> None:
        """Set whether agent is currently turning."""
        self._body.isTurning = value
    
    @property
    def last_action_time(self) -> float:
        """Get time since last action."""
        return self._body.lastActionTime
    
    @last_action_time.setter
    def last_action_time(self, value: float) -> None:
        """Set time since last action."""
        self._body.lastActionTime = value
    
    def reset(self) -> None:
        """Reset agent body to initial state.
        
        Examples:
            >>> body.reset()
        """
        self._body.reset()
    
    def __repr__(self) -> str:
        """String representation of agent body."""
        return f"<AgentBody: pos=({self.x}, {self.y}), orientation={self.orientation}>"
    
    def __str__(self) -> str:
        """String representation of agent body."""
        return f"AgentBody(x={self.x}, y={self.y}, orientation={self.orientation})"
class SensoryPercept:
    """Container for sensory percept data from all modalities.
    
    The SensoryPercept class aggregates sensory data from vision, touch,
    internal signals, and audio into a single object for easy access.
    
    Examples:
        >>> percept = SensoryPercept()
        >>> percept.set_vision(vision_data)
        >>> percept.set_touch(touch_data)
        >>> vision = percept.get_vision()
        >>> width, height = percept.get_vision_width(), percept.get_vision_height()
    """
    
    def __init__(self) -> None:
        """Create an empty sensory percept."""
        self._percept = _c_extension.SensoryPercept()
    
    def get_vision(self) -> Any:
        """Get vision data.
        
        Returns:
            Vision data object or None if not set.
            
        Examples:
            >>> vision = percept.get_vision()
        """
        return self._percept.getVision()
    
    def set_vision(self, vision: Any) -> None:
        """Set vision data.
        
        Args:
            vision: Vision data object.
            
        Examples:
            >>> percept.set_vision(vision_data)
        """
        self._percept.setVision(vision)
    
    def get_vision_width(self) -> int:
        """Get vision width.
        
        Returns:
            Vision image width in pixels.
            
        Examples:
            >>> width = percept.get_vision_width()
        """
        return self._percept.getVisionWidth()
    
    def get_vision_height(self) -> int:
        """Get vision height.
        
        Returns:
            Vision image height in pixels.
            
        Examples:
            >>> height = percept.get_vision_height()
        """
        return self._percept.getVisionHeight()
    
    def get_touch(self) -> Any:
        """Get touch data.
        
        Returns:
            Touch data object or None if not set.
            
        Examples:
            >>> touch = percept.get_touch()
        """
        return self._percept.getTouch()
    
    def set_touch(self, touch: Any) -> None:
        """Set touch data.
        
        Args:
            touch: Touch data object.
            
        Examples:
            >>> percept.set_touch(touch_data)
        """
        self._percept.setTouch(touch)
    
    def get_internal(self) -> Any:
        """Get internal signals.
        
        Returns:
            Internal signals object or None if not set.
            
        Examples:
            >>> internal = percept.get_internal()
        """
        return self._percept.getInternal()
    
    def set_internal(self, internal: Any) -> None:
        """Set internal signals.
        
        Args:
            internal: Internal signals object.
            
        Examples:
            >>> percept.set_internal(internal_data)
        """
        self._percept.setInternal(internal)
    
    def get_proprioception(self) -> Any:
        """Get proprioceptive data.
        
        Returns:
            Proprioception data object or None if not set.
            
        Examples:
            >>> proprioception = percept.get_proprioception()
        """
        return self._percept.getProprioception()
    
    def set_proprioception(self, proprioception: Any) -> None:
        """Set proprioceptive data.
        
        Args:
            proprioception: Proprioception data object.
            
        Examples:
            >>> percept.set_proprioception(proprio_data)
        """
        self._percept.setProprioception(proprioception)
    
    def get_audio(self) -> Any:
        """Get audio data.
        
        Returns:
            Audio data object or None if not set.
            
        Examples:
            >>> audio = percept.get_audio()
        """
        return self._percept.getAudio()
    
    def set_audio(self, audio: Any) -> None:
        """Set audio data.
        
        Args:
            audio: Audio data object.
            
        Examples:
            >>> percept.set_audio(audio_data)
        """
        self._percept.setAudio(audio)
    
    def get_all_signals(self) -> List[Any]:
        """Get all sensory signals.
        
        Returns:
            List of all sensory signals.
            
        Examples:
            >>> signals = percept.get_all_signals()
        """
        return self._percept.getAllSignals()
    
    def get_timestamp(self) -> float:
        """Get sensor timestamp.
        
        Returns:
            Sensor timestamp in simulation time.
            
        Examples:
            >>> timestamp = percept.get_timestamp()
        """
        return self._percept.getTimestamp()
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set sensor timestamp.
        
        Args:
            timestamp: Sensor timestamp in simulation time.
            
        Examples:
            >>> percept.set_timestamp(100.0)
        """
        self._percept.setTimestamp(timestamp)
    
    def __repr__(self) -> str:
        """String representation of sensory percept."""
        return f"<SensoryPercept: timestamp={self.get_timestamp()}>"
    
    def __str__(self) -> str:
        """String representation of sensory percept."""
        return f"SensoryPercept(timestamp={self.get_timestamp()})"
class Action:
    """Action representation for motor output.
    
    The Action class represents a proposed motor action by the agent,
    including action type, parameters, and metadata.
    
    Examples:
        >>> from pynlm import ActionType
        >>> action = Action()
        >>> action.setType(ActionType.MoveForward)
        >>> action.setParameters([1.0, 0.0])  # x, y movement
        >>> print(f"Action: {action.getName()}")
        >>> cloned = action.clone()
    """
    
    def __init__(self, action_type: Optional[Any] = None, parameters: Optional[List[float]] = None) -> None:
        """Create an action.
        
        Args:
            action_type: Action type to create (optional).
            parameters: Action parameters (optional).
            
        Examples:
            >>> action = Action()
            >>> action = Action(ActionType.MoveForward)
            >>> action = Action(ActionType.MoveForward, [1.0, 0.0])
        """
        self._action = _c_extension.Action()
        
        if action_type is not None:
            self._action.setType(action_type)
        
        if parameters is not None:
            self._action.setParameters(parameters)
    
    def get_type(self) -> Any:
        """Get action type.
        
        Returns:
            Action type enum value.
            
        Examples:
            >>> action_type = action.getType()
        """
        return self._action.getType()
    
    def set_type(self, action_type: Any) -> None:
        """Set action type.
        
        Args:
            action_type: Action type enum value.
            
        Examples:
            >>> action.setType(ActionType.MoveForward)
        """
        self._action.setType(action_type)
    
    def get_parameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            List of action parameters.
            
        Examples:
            >>> params = action.getParameters()
        """
        return self._action.getParameters()
    
    def set_parameters(self, parameters: List[float]) -> None:
        """Set action parameters.
        
        Args:
            parameters: List of action parameters.
            
        Examples:
            >>> action.setParameters([1.0, 0.0])
        """
        self._action.setParameters(parameters)
    
    def get_name(self) -> str:
        """Get human-readable action name.
        
        Returns:
            Action name string.
            
        Examples:
            >>> name = action.getName()
        """
        return self._action.getName()
    
    def clone(self) -> 'Action':
        """Create a copy of this action.
        
        Returns:
            Cloned Action object.
            
        Examples:
            >>> cloned_action = action.clone()
        """
        cloned = Action()
        cloned._action = self._action.clone()
        return cloned
    
    def __repr__(self) -> str:
        """String representation of action."""
        return f"<Action: {self.get_name()}>"
    
    def __str__(self) -> str:
        """String representation of action."""
        return f"Action({self.get_name()})"
class ActionResult:
    """Action result from world execution.
    
    The ActionResult class represents the outcome of an action execution,
    including reward value, success status, and descriptive message.
    
    Examples:
        >>> result = ActionResult(1.0, True, "Moved successfully")
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = "") -> None:
        """Create an action result.
        
        Args:
            reward: Reward value from action execution.
            success: Whether the action was successful.
            message: Descriptive message about action outcome.
            
        Examples:
            >>> result = ActionResult(1.0, True)
            >>> result = ActionResult(reward=0.5, success=False, message="Collided")
        """
        self._result = _c_extension.ActionResult(reward, success, message)
    
    @property
    def reward(self) -> float:
        """Get reward value."""
        return self._result.reward
    
    @reward.setter
    def reward(self, value: float) -> None:
        """Set reward value."""
        self._result.reward = value
    
    @property
    def success(self) -> bool:
        """Get success status."""
        return self._result.success
    
    @success.setter
    def success(self, value: bool) -> None:
        """Set success status."""
        self._result.success = value
    
    @property
    def message(self) -> str:
        """Get descriptive message."""
        return self._result.message
    
    @message.setter
    def message(self, value: str) -> None:
        """Set descriptive message."""
        self._result.message = value
    
    def __repr__(self) -> str:
        """String representation of action result."""
        return f"<ActionResult: reward={self.reward}, success={self.success}>"
    
    def __str__(self) -> str:
        """String representation of action result."""
        return f"ActionResult(reward={self.reward}, success={self.success})"
# Convenience functions
def create_agent_brain(brain: Any) -> AgentBrain:
    """Create an agent brain interface.
    
    Args:
        brain: The Brain object to interface with.
        
    Returns:
        AgentBrain instance.
        
    Examples:
        >>> from pynlm import Brain, create_agent_brain
        >>> brain = Brain()
        >>> agent_brain = create_agent_brain(brain)
    """
    return AgentBrain(brain)
