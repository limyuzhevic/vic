'''Motor and action classes for pynlm'''

from __future__ import annotations

from typing import List, Optional, Any, Dict
from .core.types import NeuronId, SimulationStep, Timestamp
from .core.enums import ActionType, MotorCommand, WorldObjectType
from .agent.AgentBody import AgentBody


class Action:
    """Action representation for motor output.
    
    The Action class represents an agent's intended action in the world,
    containing type information and parameters for execution.
    """
    
    def __init__(self, action_type: ActionType = ActionType.WAIT):
        """Initialize an Action instance.
        
        Args:
            action_type: Type of action to perform. Defaults to WAIT.
        """
        self.action_type: ActionType = action_type
        self.parameters: List[float] = []
        self.timestamp: Timestamp = 0.0
        self.id: int = 0
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, 
                 parameters: Optional[List[float]] = None):
        """Initialize an Action instance with parameters.
        
        Args:
            action_type: Type of action to perform. Defaults to WAIT.
            parameters: Optional list of action parameters.
        """
        self.action_type: ActionType = action_type
        self.parameters: List[float] = parameters if parameters is not None else []
        self.timestamp: Timestamp = 0.0
        self.id: int = 0
    
    def setType(self, action_type: ActionType):
        """Set the action type.
        
        Args:
            action_type: New action type.
        """
        self.action_type = action_type
    
    def getType(self) -> ActionType:
        """Get the action type.
        
        Returns:
            Current action type.
        """
        return self.action_type
    
    def setParameters(self, params: List[float]):
        """Set action parameters.
        
        Args:
            params: New action parameters.
        """
        self.parameters = params.copy()
    
    def getParameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            Action parameters.
        """
        return self.parameters.copy()
    
    def getName(self) -> str:
        """Get human-readable name of the action.
        
        Returns:
            Action name.
        """
        return str(self.action_type)
    
    def clone(self) -> 'Action':
        """Create a copy of this action.
        
        Returns:
            Copy of this action.
        """
        return Action(self.action_type, self.parameters.copy())
    
    def __repr__(self) -> str:
        """Get string representation of the action.
        
        Returns:
            String representation of the action.
        """
        return f"<Action {self.action_type.name} (params: {len(self.parameters)})>"


class WorldObject:
    """World object representation.
    
    The WorldObject class represents objects in the simulated world,
    including their position, type, and properties.
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: WorldObjectType = WorldObjectType.EMPTY,
                 value: float = 0.0, radius: float = 0.5):
        """Initialize a WorldObject instance.
        
        Args:
            x: X-coordinate of object center.
            y: Y-coordinate of object center.
            obj_type: Type of object. Defaults to EMPTY.
            value: Object value (e.g., resource amount).
            radius: Object radius for collision detection.
        """
        self.x: float = x
        self.y: float = y
        self.type: WorldObjectType = obj_type
        self.value: float = value
        self.radius: float = radius
        self.active: bool = True
        self.timestamp: Timestamp = 0.0
    
    def __repr__(self) -> str:
        """Get string representation of the world object.
        
        Returns:
            String representation of the world object.
        """
        return f"<WorldObject ({self.type.name} at ({self.x:.1f}, {self.y:.1f}), " \
               f"value: {self.value}, active: {self.active})>"


class AgentBody:
    """Agent body state - contains physical properties and internal state.
    
    This class represents the agent's physical body state in the world,
    including position, velocity, orientation, and physiological state.
    """
    
    def __init__(self):
        """Initialize an AgentBody instance."""
        # Position in world
        self.x: float = 0.0
        self.y: float = 0.0
        
        # Orientation (radians)
        self.orientation: float = 0.0
        
        # Velocity
        self.velocityX: float = 0.0
        self.velocityY: float = 0.0
        self.angularVelocity: float = 0.0
        
        # Internal state
        self.energy: float = 1.0  # Energy level (0-1)
        self.health: float = 1.0  # Health (0-1)
        self.age: float = 0.0  # Developmental age in simulation steps
        
        # Movement state
        self.isMoving: bool = False
        self.isTurning: bool = False
        self.lastActionTime: float = -1000.0
    
    def __repr__(self) -> str:
        """Get string representation of the agent body.
        
        Returns:
            String representation of the agent body.
        """
        return f"<AgentBody (pos: ({self.x:.1f}, {self.y:.1f}), " \
               f"orientation: {self.orientation:.2f}, energy: {self.energy:.2f})>"


class ActionResult:
    """Action result from world perspective.
    
    The ActionResult class represents the result of an action executed
    by an agent in the world, including reward and success information.
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False,
                 message: str = ""):
        """Initialize an ActionResult instance.
        
        Args:
            reward: Reward received from action.
            success: Whether action was successful.
            message: Optional message describing action result.
        """
        self.reward: float = reward
        self.success: bool = success
        self.message: str = message
    
    def __repr__(self) -> str:
        """Get string representation of the action result.
        
        Returns:
            String representation of the action result.
        """
        return f"<ActionResult (reward: {self.reward:.2f}, " \
               f"success: {self.success}, message: '{self.message}')>"


class SensoryPercept:
    """Sensory percept data.
    
    The SensoryPercept class represents processed sensory data
    from all modalities, providing a unified view of the agent's
    perception of the world.
    """
    
    def __init__(self, vision_width: int = 0, vision_height: int = 0):
        """Initialize a SensoryPercept instance.
        
        Args:
            vision_width: Width of vision field.
            vision_height: Height of vision field.
        """
        # Vision
        self.vision: Optional[Vision] = None
        if vision_width > 0 and vision_height > 0:
            self.vision = Vision(vision_width, vision_height)
        
        # Touch/perception
        self.touch: List[float] = []
        
        # Internal signals
        self.internal: Optional[InternalSignals] = None
        
        # Proprioception (body position in space)
        self.proprioception: List[float] = []
        
        # Audio
        self.audio: Optional[Audio] = None
        
        # Timestamp
        self.timestamp: Timestamp = 0.0
    
    def getVision(self) -> Optional[Vision]:
        """Get vision percept.
        
        Returns:
            Vision percept.
        """
        return self.vision
    
    def setVision(self, vision: Vision):
        """Set vision percept.
        
        Args:
            vision: Vision percept to set.
        """
        self.vision = vision
    
    def getVisionWidth(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width.
        """
        return self.vision.getWidth() if self.vision else 0
    
    def getVisionHeight(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height.
        """
        return self.vision.getHeight() if self.vision else 0
    
    def getTouch(self) -> List[float]:
        """Get touch percept.
        
        Returns:
            Touch percept data.
        """
        return self.touch.copy()
    
    def setTouch(self, touch: List[float]):
        """Set touch percept.
        
        Args:
            touch: Touch percept data.
        """
        self.touch = touch.copy()
    
    def getInternal(self) -> Optional[InternalSignals]:
        """Get internal signals percept.
        
        Returns:
            Internal signals percept.
        """
        return self.internal
    
    def setInternal(self, internal: InternalSignals):
        """Set internal signals percept.
        
        Args:
            internal: Internal signals percept to set.
        """
        self.internal = internal
    
    def getProprioception(self) -> List[float]:
        """Get proprioception percept.
        
        Returns:
            Proprioception percept data.
        """
        return self.proprioception.copy()
    
    def setProprioception(self, proprioception: List[float]):
        """Set proprioception percept.
        
        Args:
            proprioception: Proprioception percept data.
        """
        self.proprioception = proprioception.copy()
    
    def getAudio(self) -> Optional[Audio]:
        """Get audio percept.
        
        Returns:
            Audio percept.
        """
        return self.audio
    
    def setAudio(self, audio: Audio):
        """Set audio percept.
        
        Args:
            audio: Audio percept to set.
        """
        self.audio = audio
    
    def getAllSignals(self) -> Dict[str, Any]:
        """Get all sensory signals as a dictionary.
        
        Returns:
            Dictionary of all sensory signals.
        """
        signals = {
            'vision': self.vision,
            'touch': self.touch,
            'internal': self.internal,
            'proprioception': self.proprioception,
            'audio': self.audio,
        }
        return signals
    
    def getTimestamp(self) -> Timestamp:
        """Get percept timestamp.
        
        Returns:
            Percept timestamp.
        """
        return self.timestamp
    
    def setTimestamp(self, timestamp: Timestamp):
        """Set percept timestamp.
        
        Args:
            timestamp: New timestamp.
        """
        self.timestamp = timestamp
    
    def clearAllSignals(self):
        """Clear all sensory signals."""
        if self.vision:
            self.vision.data = [0.0] * self.vision.getDimensions()
        
        self.touch.clear()
        
        if self.internal:
            self.internal.clearSignals()
        
        self.proprioception.clear()
        
        if self.audio:
            self.audio.data = [0.0] * self.audio.getNumSamples()
    
    def __repr__(self) -> str:
        """Get string representation of the sensory percept.
        
        Returns:
            String representation of the sensory percept.
        """
        signals = []
        if self.vision:
            signals.append(f"vision({self.vision.getWidth()}x{self.vision.getHeight()})")
        if self.touch:
            signals.append(f"touch({len(self.touch)})")
        if self.internal:
            signals.append(f"internal")
        if self.proprioception:
            signals.append(f"proprioception({len(self.proprioception)})")
        if self.audio:
            signals.append(f"audio({self.audio.getSampleRate()}Hz)")
        
        return f"<SensoryPercept ({', '.join(signals)}, timestamp: {self.timestamp:.2f}s)>"


__all__ = ['Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept']