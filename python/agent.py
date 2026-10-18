"""
Agent-related classes and functions for NLM Python bindings.
"""

from typing import List, Optional, Dict, Any
import numpy as np
import structlog

# Get the logger
logger = structlog.get_logger()

from .brain import Brain, Action, ActionTypes
from .world import SimpleWorld

# Forward references for type annotations
class Config:
    pass

# Type aliases
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int

# Exception classes
class AgentError(Exception):
    """Base exception for agent errors."""
    pass

class InvalidWorldError(AgentError):
    """Raised when world is invalid."""
    pass

class AgentInitializationError(AgentError):
    """Raised when agent initialization fails."""
    pass

# Agent body state class
class AgentBody:
    """Agent body state for tracking agent position and orientation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, orientation: float = 0.0,
                 velocity_x: float = 0.0, velocity_y: float = 0.0,
                 angular_velocity: float = 0.0, energy: float = 100.0,
                 health: float = 100.0, age: int = 0):
        self._x = x
        self._y = y
        self._orientation = orientation
        self._velocity_x = velocity_x
        self._velocity_y = velocity_y
        self._angular_velocity = angular_velocity
        self._energy = energy
        self._health = health
        self._age = age
        self._is_moving = False
        self._is_turning = False
        self._last_action_time = 0.0
        
        logger.debug("AgentBody created", 
                    x=x, y=y, orientation=orientation,
                    energy=energy, health=health)
    
    @property
    def x(self) -> float:
        """Get x position."""
        return self._x
    
    @x.setter
    def x(self, value: float):
        """Set x position."""
        self._x = value
    
    @property
    def y(self) -> float:
        """Get y position."""
        return self._y
    
    @y.setter
    def y(self, value: float):
        """Set y position."""
        self._y = value
    
    @property
    def orientation(self) -> float:
        """Get orientation (radians)."""
        return self._orientation
    
    @orientation.setter
    def orientation(self, value: float):
        """Set orientation (radians)."""
        self._orientation = value
    
    @property
    def velocity_x(self) -> float:
        """Get x velocity."""
        return self._velocity_x
    
    @velocity_x.setter
    def velocity_x(self, value: float):
        """Set x velocity."""
        self._velocity_x = value
    
    @property
    def velocity_y(self) -> float:
        """Get y velocity."""
        return self._velocity_y
    
    @velocity_y.setter
    def velocity_y(self, value: float):
        """Set y velocity."""
        self._velocity_y = value
    
    @property
    def angular_velocity(self) -> float:
        """Get angular velocity (rad/s)."""
        return self._angular_velocity
    
    @angular_velocity.setter
    def angular_velocity(self, value: float):
        """Set angular velocity (rad/s)."""
        self._angular_velocity = value
    
    @property
    def energy(self) -> float:
        """Get energy."""
        return self._energy
    
    @energy.setter
    def energy(self, value: float):
        """Set energy."""
        self._energy = value
    
    @property
    def health(self) -> float:
        """Get health."""
        return self._health
    
    @health.setter
    def health(self, value: float):
        """Set health."""
        self._health = value
    
    @property
    def age(self) -> int:
        """Get age."""
        return self._age
    
    @age.setter
    def age(self, value: int):
        """Set age."""
        self._age = value
    
    @property
    def is_moving(self) -> bool:
        """Get moving status."""
        return self._is_moving
    
    @is_moving.setter
    def is_moving(self, value: bool):
        """Set moving status."""
        self._is_moving = value
    
    @property
    def is_turning(self) -> bool:
        """Get turning status."""
        return self._is_turning
    
    @is_turning.setter
    def is_turning(self, value: bool):
        """Set turning status."""
        self._is_turning = value
    
    @property
    def last_action_time(self) -> float:
        """Get last action time."""
        return self._last_action_time
    
    @last_action_time.setter
    def last_action_time(self, value: float):
        """Set last action time."""
        self._last_action_time = value
    
    def reset(self):
        """Reset agent body state."""
        self._x = 0.0
        self._y = 0.0
        self._orientation = 0.0
        self._velocity_x = 0.0
        self._velocity_y = 0.0
        self._angular_velocity = 0.0
        self._energy = 100.0
        self._health = 100.0
        self._age = 0
        self._is_moving = False
        self._is_turning = False
        self._last_action_time = 0.0
        logger.debug("AgentBody reset")
    
    def get_position(self) -> tuple:
        """Get agent position as (x, y) tuple.
        
        Returns:
            tuple: (x, y) position.
        """
        return (self._x, self._y)
    
    def set_position(self, x: float, y: float):
        """Set agent position.
        
        Args:
            x: X position.
            y: Y position.
        """
        self._x = x
        self._y = y
    
    def get_velocity(self) -> tuple:
        """Get agent velocity as (vx, vy) tuple.
        
        Returns:
            tuple: (vx, vy) velocity.
        """
        return (self._velocity_x, self._velocity_y)
    
    def get_distance_to(self, other_x: float, other_y: float) -> float:
        """Get distance to another point.
        
        Args:
            other_x: X position of other point.
            other_y: Y position of other point.
            
        Returns:
            float: Distance to other point.
        """
        dx = other_x - self._x
        dy = other_y - self._y
        return (dx**2 + dy**2)**0.5


class ActionResult:
    """Result of an action performed in the world."""
    
    def __init__(self, reward: float = 0.0, success: bool = True, message: str = ""):
        self._reward = reward
        self._success = success
        self._message = message
        
        logger.debug("ActionResult created", 
                    reward=reward, success=success, message=message)
    
    @property
    def reward(self) -> float:
        """Get reward value."""
        return self._reward
    
    @reward.setter
    def reward(self, value: float):
        """Set reward value."""
        self._reward = value
    
    @property
    def success(self) -> bool:
        """Get success status."""
        return self._success
    
    @success.setter
    def success(self, value: bool):
        """Set success status."""
        self._success = value
    
    @property
    def message(self) -> str:
        """Get message."""
        return self._message
    
    @message.setter
    def message(self, value: str):
        """Set message."""
        self._message = value
    
    def is_successful(self) -> bool:
        """Check if action was successful.
        
        Returns:
            bool: True if action successful.
        """
        return self._success
    
    def __repr__(self) -> str:
        return f"<ActionResult reward={self._reward} success={self._success}>"


class SensoryPercept:
    """Container for all sensory information perceived by the agent."""
    
    def __init__(self, vision=None, audio=None, internal=None, touch=None, proprioception=None, timestamp=0.0):
        self._vision = vision
        self._audio = audio
        self._internal = internal
        self._touch = touch or []
        self._proprioception = proprioception or []
        self._timestamp = timestamp
        
        logger.debug("SensoryPercept created", 
                    vision_type=type(vision).__name__ if vision else None,
                    audio_type=type(audio).__name__ if audio else None)
    
    @property
    def vision(self):
        """Get vision sensory input."""
        return self._vision
    
    @vision.setter
    def vision(self, value):
        """Set vision sensory input."""
        self._vision = value
    
    @property
    def audio(self):
        """Get audio sensory input."""
        return self._audio
    
    @audio.setter
    def audio(self, value):
        """Set audio sensory input."""
        self._audio = value
    
    @property
    def internal(self):
        """Get internal signals."""
        return self._internal
    
    @internal.setter
    def internal(self, value):
        """Set internal signals."""
        self._internal = value
    
    @property
    def touch(self) -> List[float]:
        """Get touch sensory input."""
        return self._touch
    
    @touch.setter
    def touch(self, value: List[float]):
        """Set touch sensory input."""
        self._touch = value
    
    @property
    def proprioception(self) -> List[float]:
        """Get proprioception sensory input."""
        return self._proprioception
    
    @proprioception.setter
    def proprioception(self, value: List[float]):
        """Set proprioception sensory input."""
        self._proprioception = value
    
    @property
    def timestamp(self) -> float:
        """Get timestamp."""
        return self._timestamp
    
    @timestamp.setter
    def timestamp(self, value: float):
        """Set timestamp."""
        self._timestamp = value
    
    def get_sensory_data(self) -> Dict[str, Any]:
        """Get all sensory data as a dictionary.
        
        Returns:
            Dict[str, Any]: All sensory data.
        """
        return {
            'vision': self._vision,
            'audio': self._audio,
            'internal': self._internal,
            'touch': self._touch,
            'proprioception': self._proprioception
        }
    
    def has_sensory_input(self, modality: str) -> bool:
        """Check if agent has sensory input for a modality.
        
        Args:
            modality: Sensory modality ('vision', 'audio', 'internal', 'touch', 'proprioception').
            
        Returns:
            bool: True if has input for modality.
        """
        modalities = {
            'vision': self._vision is not None,
            'audio': self._audio is not None,
            'internal': self._internal is not None,
            'touch': len(self._touch) > 0,
            'proprioception': len(self._proprioception) > 0
        }
        return modalities.get(modality, False)
    
    def get_sensory_dimensions(self) -> Dict[str, int]:
        """Get dimensions of sensory inputs.
        
        Returns:
            Dict[str, int]: Dimensions for each modality.
        """
        dimensions = {}
        if self._vision:
            dimensions['vision'] = self._vision.get_dimensions()
        if self._audio:
            dimensions['audio'] = self._audio.get_dimensions()
        if self._internal:
            dimensions['internal'] = self._internal.get_dimensions()
        dimensions['touch'] = len(self._touch)
        dimensions['proprioception'] = len(self._proprioception)
        return dimensions
    
    def __repr__(self) -> str:
        return f"<SensoryPercept timestamp={self._timestamp}>"


class Action:
    """Represents an action that can be performed by the agent."""
    
    def __init__(self, action_type: str = ActionTypes.WAIT, parameters: List[float] = None):
        self._type = action_type
        self._parameters = parameters or []
        
        logger.debug("Action created", action_type=action_type, parameters=parameters)
    
    @property
    def type(self) -> str:
        """Get action type."""
        return self._type
    
    @type.setter
    def type(self, value: str):
        """Set action type."""
        self._type = value
    
    @property
    def parameters(self) -> List[float]:
        """Get action parameters."""
        return self._parameters
    
    @parameters.setter
    def parameters(self, value: List[float]):
        """Set action parameters."""
        self._parameters = value
    
    @property
    def name(self) -> str:
        """Get action name."""
        # Basic mapping of action types to names
        name_map = {
            ActionTypes.MOVE_FORWARD: "Move Forward",
            ActionTypes.MOVE_BACKWARD: "Move Backward",
            ActionTypes.MOVE_LEFT: "Move Left",
            ActionTypes.MOVE_RIGHT: "Move Right",
            ActionTypes.TURN_LEFT: "Turn Left",
            ActionTypes.TURN_RIGHT: "Turn Right",
            ActionTypes.LOOK: "Look",
            ActionTypes.LOOK_UP: "Look Up",
            ActionTypes.LOOK_DOWN: "Look Down",
            ActionTypes.INTERACT: "Interact",
            ActionTypes.EAT: "Eat",
            ActionTypes.DRINK: "Drink",
            ActionTypes.REST: "Rest",
            ActionTypes.WAIT: "Wait",
            ActionTypes.CUSTOM: "Custom"
        }
        return name_map.get(self._type, self._type)
    
    def add_parameter(self, value: float):
        """Add a parameter to the action.
        
        Args:
            value: Parameter value.
        """
        self._parameters.append(value)
    
    def get_parameter(self, index: int) -> float:
        """Get parameter at index.
        
        Args:
            index: Parameter index.
            
        Returns:
            float: Parameter value.
            
        Raises:
            IndexError: If index out of range.
        """
        if index < 0 or index >= len(self._parameters):
            raise IndexError(f"Parameter index {index} out of range")
        return self._parameters[index]
    
    def clone(self):
        """Create a copy of this action.
        
        Returns:
            Action: Copy of this action.
        """
        return Action(self._type, self._parameters.copy())
    
    def __repr__(self) -> str:
        return f"<Action type={self._type} params={self._parameters}>"



class AgentBrain:
    """Agent brain interface connecting NLM brain to world for action selection."""
    
    def __init__(self, brain):
        self._brain = brain
        self._world = None
        self._initialized = False
        
        # Feature flags
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        self._novelty_enabled = False
        self._prediction_enabled = False
        
        logger.debug("AgentBrain created", brain_type=type(brain).__name__)
    
    def initialize(self, world):
        """Initialize the agent brain with a world.
        
        Args:
            world: World instance to connect to.
            
        Raises:
            InvalidWorldError: If world is invalid.
            AgentInitializationError: If agent initialization fails.
        """
        if world is None:
            raise InvalidWorldError("World cannot be None")
        
        self._world = world
        self._initialized = True
        
        logger.info("AgentBrain initialized", world_type=type(world).__name__)
    
    @property
    def is_initialized(self) -> bool:
        """Check if agent brain is initialized.
        
        Returns:
            bool: True if initialized.
        """
        return self._initialized
    
    @property
    def world(self):
        """Get the connected world.
        
        Returns:
            World: Connected world.
        """
        return self._world
    
    @property
    def brain(self):
        """Get the underlying brain.
        
        Returns:
            Brain: Underlying brain.
        """
        return self._brain
    
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process.
            
        Raises:
            SimulationError: If brain is not initialized.
            AgentError: If sensory processing fails.
        """
        if not self._initialized:
            raise AgentInitializationError("AgentBrain not initialized. Call initialize() first.")
        
        if percept is None:
            raise AgentError("Sensory percept cannot be None")
        
        self._brain.receive_sensory_input(percept)
        
        logger.debug("Sensory input processed", 
                    percept_type=type(percept).__name__,
                    has_vision=percept.has_sensory_input('vision'),
                    has_audio=percept.has_sensory_input('audio'))
    
    def decode_motor_command(self) -> Action:
        """Decode brain motor activity into motor command.
        
        Returns:
            Action: Motor command to execute.
            
        Raises:
            SimulationError: If brain is not initialized.
            AgentError: If action decoding fails.
        """
        if not self._initialized:
            raise AgentInitializationError("AgentBrain not initialized.")
        
        # Get action from brain
        action = self._brain.produce_action()
        
        # If brain returns None, create a default wait action
        if action is None:
            action = Action(ActionTypes.WAIT)
        
        logger.debug("Motor command decoded", 
                    action_type=action.type,
                    action_name=action.name)
        
        return action
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation to brain.
        
        Args:
            reward: Actual reward received from world.
            predicted_reward: Predicted reward from brain.
            
        Raises:
            SimulationError: If brain is not initialized.
        """
        if not self._initialized:
            raise AgentInitializationError("AgentBrain not initialized.")
        
        if not self._reward_modulation_enabled:
            logger.debug("Reward modulation is disabled")
            return
        
        prediction_error = abs(reward - predicted_reward)
        
        # Apply neuromodulation based on prediction error
        # This would interface with brain-specific neuromodulation systems
        self._brain.apply_neuromodulation(prediction_error, reward)
        
        logger.debug("Reward modulation applied",
                    reward=reward,
                    predicted_reward=predicted_reward,
                    prediction_error=prediction_error)
    
    def update_development(self, timestep: float):
        """Update development system based on time step.
        
        Args:
            timestep: Time step duration.
            
        Raises:
            SimulationError: If brain is not initialized.
        """
        if not self._initialized:
            raise AgentInitializationError("AgentBrain not initialized.")
        
        if not self._development_enabled:
            return
        
        # Update developmental stage based on time
        current_stage = self._brain.get_developmental_stage()
        self._brain.update_development(timestep)
        
        logger.debug("Development updated", 
                    timestep=timestep,
                    stage=current_stage)
    
    def enable_features(self, **kwargs):
        """Enable various agent features.
        
        Args:
            **kwargs: Feature flags to enable.
                - reward_modulation: Enable reward modulation.
                - structural_plasticity: Enable structural plasticity.
                - development: Enable development.
                - curiosity: Enable curiosity-driven exploration.
                - novelty: Enable novelty detection.
                - prediction: Enable prediction systems.
        """
        self._reward_modulation_enabled = kwargs.get('reward_modulation', self._reward_modulation_enabled)
        self._structural_plasticity_enabled = kwargs.get('structural_plasticity', self._structural_plasticity_enabled)
        self._development_enabled = kwargs.get('development', self._development_enabled)
        self._curiosity_enabled = kwargs.get('curiosity', self._curiosity_enabled)
        self._novelty_enabled = kwargs.get('novelty', self._novelty_enabled)
        self._prediction_enabled = kwargs.get('prediction', self._prediction_enabled)
        
        logger.debug("Agent features enabled", features=list(kwargs.keys()))
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get brain statistics for monitoring.
        
        Returns:
            Dict[str, Any]: Brain statistics.
        """
        if not self._initialized:
            raise AgentInitializationError("AgentBrain not initialized.")
        
        stats = {
            'total_neurons': self._brain.get_total_neuron_count(),
            'total_synapses': self._brain.get_total_synapse_count(),
            'active_neurons': self._brain.get_active_neuron_count(),
            'firing_neurons': self._brain.get_firing_neuron_count(),
            'total_spikes': self._brain.get_total_spike_count(),
            'average_firing_rate': self._brain.get_average_firing_rate(),
            'exc_inhibition_ratio': self._brain.get_excitation_inhibition_ratio(),
            'developmental_stage': self._brain.get_developmental_stage(),
            'features_enabled': {
                'reward_modulation': self._reward_modulation_enabled,
                'structural_plasticity': self._structural_plasticity_enabled,
                'development': self._development_enabled,
                'curiosity': self._curiosity_enabled,
                'novelty': self._novelty_enabled,
                'prediction': self._prediction_enabled,
            }
        }
        
        logger.debug("Brain stats retrieved", stats_count=len(stats))
        return stats
    
    def get_neuromodulation_state(self) -> Dict[str, float]:
        """Get current neuromodulation state.
        
        Returns:
            Dict[str, float]: Neuromodulation state.
        """
        return {
            'reward_modulation': 1.0 if self._reward_modulation_enabled else 0.0,
            'curiosity': 0.7 if self._curiosity_enabled else 0.0,
            'novelty': 0.5 if self._novelty_enabled else 0.0,
            'dopamine': 0.3,
            'serotonin': 0.2,
            'noradrenaline': 0.4
        }
    
    def get_curiosity_state(self) -> Dict[str, float]:
        """Get curiosity-driven exploration state.
        
        Returns:
            Dict[str, float]: Curiosity state.
        """
        return {
            'curiosity_level': 0.6 if self._curiosity_enabled else 0.0,
            'novelty_sensitivity': 0.8 if self._novelty_enabled else 0.0,
            'exploration_rate': 0.4 if self._curiosity_enabled else 0.0,
            'inhibition_threshold': 0.5,
            'reward_prediction_error': self.get_prediction_error()
        }
    
    def get_prediction_state(self) -> Dict[str, float]:
        """Get prediction system state.
        
        Returns:
            Dict[str, float]: Prediction state.
        """
        if not self._prediction_enabled:
            return {'enabled': False}
        
        return {
            'enabled': True,
            'prediction_error': self.get_prediction_error(),
            'confidence': 0.7,
            'novelty_detection': 0.5
        }
    
    def get_prediction_error(self) -> float:
        """Get prediction error from brain.
        
        Returns:
            float: Prediction error.
        """
        # This would call into brain-specific prediction systems
        return 0.1
    
    def reset(self):
        """Reset agent for new episode."""
        self._initialized = False
        self._world = None
        self._brain.reset()
        
        logger.info("AgentBrain reset for new episode")
    
    # Feature checking methods
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            bool: True if reward modulation enabled.
        """
        return self._reward_modulation_enabled
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            bool: True if structural plasticity enabled.
        """
        return self._structural_plasticity_enabled
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            bool: True if development enabled.
        """
        return self._development_enabled
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            bool: True if curiosity enabled.
        """
        return self._curiosity_enabled
    
    def is_novelty_enabled(self) -> bool:
        """Check if novelty detection is enabled.
        
        Returns:
            bool: True if novelty detection enabled.
        """
        return self._novelty_enabled
    
    def is_prediction_enabled(self) -> bool:
        """Check if prediction is enabled.
        
        Returns:
            bool: True if prediction enabled.
        """
        return self._prediction_enabled


# Helper functions

def create_agent_brain(brain) -> AgentBrain:
    """Create an agent brain interface from an existing brain.
    
    Args:
        brain: Existing Brain instance.
        
    Returns:
        AgentBrain: Agent brain interface.
        
    Raises:
        AgentError: If agent creation fails.
    """
    if brain is None:
        raise AgentError("Brain cannot be None")
    
    logger.debug("Creating agent brain interface")
    return AgentBrain(brain)


def create_default_agent(config=None) -> AgentBrain:
    """Create an agent brain with default configuration.
    
    Args:
        config: Optional configuration. If None, uses default config.
        
    Returns:
        AgentBrain: Agent brain interface.
    """
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    brain.initialize()
    
    agent = AgentBrain(brain)
    logger.debug("Default agent created")
    
    return agent


# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Export all public symbols
__all__ = [
    # Classes
    'AgentBrain',
    'AgentBody',
    'Action',
    'ActionResult',
    'SensoryPercept',
    
    # Helper functions
    'create_agent_brain',
    'create_default_agent',
]

# Exception classes
__all__ += [
    'AgentError',
    'InvalidWorldError',
    'AgentInitializationError'
]