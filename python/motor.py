"""
NLM Action classes.

This module provides action classes for the NLM system.
The action system enables the agent to interact with the world
through motor commands and movements.
"""

import math
from typing import List, Any, Optional, Union, Tuple, Dict

# Import C++ bindings
from .bindings import (
    Action as _Action,
    ActionType,
    MotorCommand,
    WorldObject as _WorldObject,
    ActionResult as _ActionResult,
    AgentBody as _AgentBody,
)
class EnhancedAction:
    """Enhanced Action class with better error handling.
    
    This class provides a Pythonic interface to NLM actions,
    making it easier to work with motor commands through properties
    and methods that follow Python conventions.
    
    The enhanced action provides:
    - Better error handling with custom exceptions
    - Convenience methods for action manipulation
    - Type-safe action access
    - Action validation
    - Action result handling
    
    Example:
        >>> # Create action from type
        >>> action = EnhancedAction(ActionType.MoveForward)
        >>> 
        >>> # Set parameters
        >>> action.setParameters([1.0, 0.5])
        >>> 
        >>> # Access action properties
        >>> print(f"Action: {action.getName()}")
        >>> print(f"Parameters: {action.getParameters()}")
    """
    
    def __init__(self, action_type: Optional[Any] = None, parameters: Optional[List[float]] = None):
        """Initialize action.
        
        Args:
            action_type: Action type (ActionType enum). If None, default action type is used.
            parameters: Action parameters as list of floats. If None, empty list is used.
        """
        try:
            if action_type is not None and parameters is not None:
                self._action = _Action(action_type, parameters)
            elif action_type is not None:
                self._action = _Action(action_type)
            elif parameters is not None:
                self._action = _Action(parameters)
            else:
                self._action = _Action()
        except Exception as e:
            raise ActionError(f"Failed to create action: {e}")
    
    def getType(self) -> Any:
        """Get action type.
        
        Returns:
            Action type.
        """
        try:
            return self._action.getType()
        except Exception as e:
            raise ActionError(f"Failed to get action type: {e}")
    
    def setType(self, action_type: Any) -> None:
        """Set action type.
        
        Args:
            action_type: Action type to set.
        """
        try:
            self._action.setType(action_type)
        except Exception as e:
            raise ActionError(f"Failed to set action type: {e}")
    
    def getParameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            List of action parameters.
        """
        try:
            return self._action.getParameters()
        except Exception as e:
            raise ActionError(f"Failed to get action parameters: {e}")
    
    def setParameters(self, parameters: List[float]) -> None:
        """Set action parameters.
        
        Args:
            parameters: Action parameters to set.
        """
        try:
            self._action.setParameters(parameters)
        except Exception as e:
            raise ActionError(f"Failed to set action parameters: {e}")
    
    def getName(self) -> str:
        """Get action name.
        
        Returns:
            Action name as string.
        """
        try:
            return self._action.getName()
        except Exception as e:
            raise ActionError(f"Failed to get action name: {e}")
    
    def clone(self) -> 'EnhancedAction':
        """Clone action.
        
        Returns:
            Cloned action.
        """
        try:
            cloned_action = _Action()
            # Copy type and parameters
            cloned_action.setType(self.getType())
            cloned_action.setParameters(self.getParameters())
            return EnhancedAction(cloned_action)
        except Exception as e:
            raise ActionError(f"Failed to clone action: {e}")
    
    def __repr__(self) -> str:
        return f"EnhancedAction(type={self.getType()}, params={self.getParameters()})"
    
    def __str__(self) -> str:
        return f"{self.getName()}(params={self.getParameters()})"
class Action(EnhancedAction):
    """Action class for NLM.
    
    This class provides action representation for motor output in the NLM system.
    Actions represent movements, interactions, or other motor behaviors performed
    by the agent in the world.
    
    Example:
        >>> # Create action from type
        >>> action = Action(ActionType.MoveForward)
        >>> 
        >>> # Create action with parameters
        >>> action = Action(ActionType.MoveBackward, [1.0, 0.5])
        >>> 
        >>> # Access action properties
        >>> print(f"Action: {action.getName()}")
        >>> print(f"Parameters: {action.getParameters()}")
    """
    
    def __init__(self, action_type: Optional[Any] = None, parameters: Optional[List[float]] = None):
        """Initialize action."""
        super().__init__(action_type, parameters)
class EnhancedWorldObject:
    """Enhanced WorldObject class with better error handling.
    
    This class provides a Pythonic interface to NLM world objects,
    making it easier to work with world objects through properties
    and methods that follow Python conventions.
    
    The enhanced world object provides:
    - Better error handling with custom exceptions
    - Convenience methods for object manipulation
    - Type-safe object access
    - Object validation
    
    Example:
        >>> # Create world object
        >>> obj = EnhancedWorldObject(10.0, 10.0, WorldObjectType.Resource, 1.0)
        >>> 
        >>> # Access object properties
        >>> print(f"Position: ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        >>> print(f"Value: {obj.value}")
        >>> 
        >>> # Check if object is active
        >>> if obj.active:
        ...     print("Object is active")
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: Any = None, value: float = 0.0, radius: float = 0.5):
        """Initialize world object.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            obj_type: Object type (WorldObjectType enum).
            value: Object value.
            radius: Object radius.
        """
        try:
            if obj_type is not None:
                self._object = _WorldObject(x, y, obj_type, value, radius)
            else:
                self._object = _WorldObject()
                self._object.x = x
                self._object.y = y
                if obj_type is not None:
                    self._object.type = obj_type
                self._object.value = value
                self._object.radius = radius
        except Exception as e:
            raise WorldError(f"Failed to create world object: {e}")
    
    # Property access methods
    
    def getX(self) -> float:
        """Get X coordinate.
        
        Returns:
            X coordinate.
        """
        try:
            return self._object.x
        except Exception as e:
            raise WorldError(f"Failed to get X coordinate: {e}")
    
    def setX(self, x: float) -> None:
        """Set X coordinate.
        
        Args:
            x: X coordinate.
        """
        try:
            self._object.x = x
        except Exception as e:
            raise WorldError(f"Failed to set X coordinate: {e}")
    
    def getY(self) -> float:
        """Get Y coordinate.
        
        Returns:
            Y coordinate.
        """
        try:
            return self._object.y
        except Exception as e:
            raise WorldError(f"Failed to get Y coordinate: {e}")
    
    def setY(self, y: float) -> None:
        """Set Y coordinate.
        
        Args:
            y: Y coordinate.
        """
        try:
            self._object.y = y
        except Exception as e:
            raise WorldError(f"Failed to set Y coordinate: {e}")
    
    def getRadius(self) -> float:
        """Get radius.
        
        Returns:
            Radius.
        """
        try:
            return self._object.radius
        except Exception as e:
            raise WorldError(f"Failed to get radius: {e}")
    
    def setRadius(self, radius: float) -> None:
        """Set radius.
        
        Args:
            radius: Radius.
        """
        try:
            self._object.radius = radius
        except Exception as e:
            raise WorldError(f"Failed to set radius: {e}")
    
    def getType(self) -> Any:
        """Get object type.
        
        Returns:
            Object type.
        """
        try:
            return self._object.type
        except Exception as e:
            raise WorldError(f"Failed to get object type: {e}")
    
    def setType(self, obj_type: Any) -> None:
        """Set object type.
        
        Args:
            obj_type: Object type.
        """
        try:
            self._object.type = obj_type
        except Exception as e:
            raise WorldError(f"Failed to set object type: {e}")
    
    def getValue(self) -> float:
        """Get value.
        
        Returns:
            Value.
        """
        try:
            return self._object.value
        except Exception as e:
            raise WorldError(f"Failed to get value: {e}")
    
    def setValue(self, value: float) -> None:
        """Set value.
        
        Args:
            value: Value.
        """
        try:
            self._object.value = value
        except Exception as e:
            raise WorldError(f"Failed to set value: {e}")
    
    def isActive(self) -> bool:
        """Check if object is active.
        
        Returns:
            True if object is active, False otherwise.
        """
        try:
            return self._object.active
        except Exception as e:
            raise WorldError(f"Failed to check object active status: {e}")
    
    def setActive(self, active: bool) -> None:
        """Set active status.
        
        Args:
            active: Active status.
        """
        try:
            self._object.active = active
        except Exception as e:
            raise WorldError(f"Failed to set active status: {e}")
    
    # Pythonic properties
    
    @property
    def x(self) -> float:
        """X coordinate."""
        return self.getX()
    
    @x.setter
    def x(self, value: float) -> None:
        self.setX(value)
    
    @property
    def y(self) -> float:
        """Y coordinate."""
        return self.getY()
    
    @y.setter
    def y(self, value: float) -> None:
        self.setY(value)
    
    @property
    def radius(self) -> float:
        """Radius."""
        return self.getRadius()
    
    @radius.setter
    def radius(self, value: float) -> None:
        self.setRadius(value)
    
    @property
    def type(self) -> Any:
        """Object type."""
        return self.getType()
    
    @type.setter
    def type(self, value: Any) -> None:
        self.setType(value)
    
    @property
    def value(self) -> float:
        """Value."""
        return self.getValue()
    
    @value.setter
    def value(self, value: float) -> None:
        self.setValue(value)
    
    @property
    def active(self) -> bool:
        """Active status."""
        return self.isActive()
    
    @active.setter
    def active(self, value: bool) -> None:
        self.setActive(value)
    
    def __repr__(self) -> str:
        return f"EnhancedWorldObject(x={self.x}, y={self.y}, type={self.type}, active={self.active})"
class WorldObject(EnhancedWorldObject):
    """WorldObject class for NLM.
    
    This class provides world object representation for the NLM system.
    World objects represent entities in the simulation world, such as
    resources, hazards, walls, markers, etc.
    
    Example:
        >>> # Create world object
        >>> obj = WorldObject(10.0, 10.0, WorldObjectType.Resource, 1.0)
        >>> 
        >>> # Access object properties
        >>> print(f"Position: ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        >>> print(f"Active: {obj.active}")
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: Any = None, value: float = 0.0, radius: float = 0.5):
        """Initialize world object."""
        super().__init__(x, y, obj_type, value, radius)
class EnhancedActionResult:
    """Enhanced ActionResult class with better error handling.
    
    This class provides a Pythonic interface to action results,
    making it easier to work with action outcomes through properties
    and methods that follow Python conventions.
    
    The enhanced action result provides:
    - Better error handling with custom exceptions
    - Convenience methods for result access
    - Type-safe result access
    - Result validation
    
    Example:
        >>> # Create action result
        >>> result = EnhancedActionResult(10.0, True, "Success")
        >>> 
        >>> # Access result properties
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
        >>> print(f"Message: {result.message}")
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """Initialize action result.
        
        Args:
            reward: Reward value.
            success: Whether action was successful.
            message: Message describing result.
        """
        try:
            self._result = _ActionResult(reward, success, message)
        except Exception as e:
            raise ActionError(f"Failed to create action result: {e}")
    
    @property
    def reward(self) -> float:
        """Reward value."""
        try:
            return self._result.reward
        except Exception as e:
            raise ActionError(f"Failed to get reward: {e}")
    
    @reward.setter
    def reward(self, value: float) -> None:
        try:
            self._result.reward = value
        except Exception as e:
            raise ActionError(f"Failed to set reward: {e}")
    
    @property
    def success(self) -> bool:
        """Success status."""
        try:
            return self._result.success
        except Exception as e:
            raise ActionError(f"Failed to get success status: {e}")
    
    @success.setter
    def success(self, value: bool) -> None:
        try:
            self._result.success = value
        except Exception as e:
            raise ActionError(f"Failed to set success status: {e}")
    
    @property
    def message(self) -> str:
        """Message."""
        try:
            return self._result.message
        except Exception as e:
            raise ActionError(f"Failed to get message: {e}")
    
    @message.setter
    def message(self, value: str) -> None:
        try:
            self._result.message = value
        except Exception as e:
            raise ActionError(f"Failed to set message: {e}")
    
    def __repr__(self) -> str:
        return f"EnhancedActionResult(reward={self.reward}, success={self.success}, message='{self.message}')"
class ActionResult(EnhancedActionResult):
    """ActionResult class for NLM.
    
    This class provides action result representation for the NLM system.
    Action results represent the outcome of motor commands applied to the world,
    including reward values, success status, and descriptive messages.
    
    Example:
        >>> # Create action result
        >>> result = ActionResult(10.0, True, "Success")
        >>> 
        >>> # Access result properties
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
        >>> print(f"Message: {result.message}")
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """Initialize action result."""
        super().__init__(reward, success, message)
class EnhancedAgentBody:
    """Enhanced AgentBody class with better error handling.
    
    This class provides a Pythonic interface to agent body state,
    making it easier to work with agent position and state through
    properties and methods that follow Python conventions.
    
    The enhanced agent body provides:
    - Better error handling with custom exceptions
    - Convenience methods for body state access
    - Type-safe state access
    - Movement and action support
    
    Example:
        >>> # Create agent body
        >>> body = EnhancedAgentBody()
        >>> 
        >>> # Set position
        >>> body.x = 10.0
        >>> body.y = 10.0
        >>> 
        >>> # Access state
        >>> print(f"Position: ({body.x}, {body.y})")
        >>> print(f"Energy: {body.energy}")
        >>> print(f"Health: {body.health}")
    """
    
    def __init__(self):
        """Initialize agent body."""
        try:
            self._body = _AgentBody()
        except Exception as e:
            raise AgentError(f"Failed to create agent body: {e}")
    
    # Property access methods
    
    def getX(self) -> float:
        """Get X coordinate.
        
        Returns:
            X coordinate.
        """
        try:
            return self._body.x
        except Exception as e:
            raise AgentError(f"Failed to get X coordinate: {e}")
    
    def setX(self, x: float) -> None:
        """Set X coordinate.
        
        Args:
            x: X coordinate.
        """
        try:
            self._body.x = x
        except Exception as e:
            raise AgentError(f"Failed to set X coordinate: {e}")
    
    def getY(self) -> float:
        """Get Y coordinate.
        
        Returns:
            Y coordinate.
        """
        try:
            return self._body.y
        except Exception as e:
            raise AgentError(f"Failed to get Y coordinate: {e}")
    
    def setY(self, y: float) -> None:
        """Set Y coordinate.
        
        Args:
            y: Y coordinate.
        """
        try:
            self._body.y = y
        except Exception as e:
            raise AgentError(f"Failed to set Y coordinate: {e}")
    
    def getOrientation(self) -> float:
        """Get orientation.
        
        Returns:
            Orientation in radians.
        """
        try:
            return self._body.orientation
        except Exception as e:
            raise AgentError(f"Failed to get orientation: {e}")
    
    def setOrientation(self, orientation: float) -> None:
        """Set orientation.
        
        Args:
            orientation: Orientation in radians.
        """
        try:
            self._body.orientation = orientation
        except Exception as e:
            raise AgentError(f"Failed to set orientation: {e}")
    
    def getVelocityX(self) -> float:
        """Get X velocity.
        
        Returns:
            X velocity.
        """
        try:
            return self._body.velocityX
        except Exception as e:
            raise AgentError(f"Failed to get X velocity: {e}")
    
    def setVelocityX(self, velocity: float) -> None:
        """Set X velocity.
        
        Args:
            velocity: X velocity.
        """
        try:
            self._body.velocityX = velocity
        except Exception as e:
            raise AgentError(f"Failed to set X velocity: {e}")
    
    def getVelocityY(self) -> float:
        """Get Y velocity.
        
        Returns:
            Y velocity.
        """
        try:
            return self._body.velocityY
        except Exception as e:
            raise AgentError(f"Failed to get Y velocity: {e}")
    
    def setVelocityY(self, velocity: float) -> None:
        """Set Y velocity.
        
        Args:
            velocity: Y velocity.
        """
        try:
            self._body.velocityY = velocity
        except Exception as e:
            raise AgentError(f"Failed to set Y velocity: {e}")
    
    def getAngularVelocity(self) -> float:
        """Get angular velocity.
        
        Returns:
            Angular velocity in radians per second.
        """
        try:
            return self._body.angularVelocity
        except Exception as e:
            raise AgentError(f"Failed to get angular velocity: {e}")
    
    def setAngularVelocity(self, angular_velocity: float) -> None:
        """Set angular velocity.
        
        Args:
            angular_velocity: Angular velocity in radians per second.
        """
        try:
            self._body.angularVelocity = angular_velocity
        except Exception as e:
            raise AgentError(f"Failed to set angular velocity: {e}")
    
    def getEnergy(self) -> float:
        """Get energy.
        
        Returns:
            Energy.
        """
        try:
            return self._body.energy
        except Exception as e:
            raise AgentError(f"Failed to get energy: {e}")
    
    def setEnergy(self, energy: float) -> None:
        """Set energy.
        
        Args:
            energy: Energy.
        """
        try:
            self._body.energy = energy
        except Exception as e:
            raise AgentError(f"Failed to set energy: {e}")
    
    def getHealth(self) -> float:
        """Get health.
        
        Returns:
            Health.
        """
        try:
            return self._body.health
        except Exception as e:
            raise AgentError(f"Failed to get health: {e}")
    
    def setHealth(self, health: float) -> None:
        """Set health.
        
        Args:
            health: Health.
        """
        try:
            self._body.health = health
        except Exception as e:
            raise AgentError(f"Failed to set health: {e}")
    
    def getAge(self) -> float:
        """Get age.
        
        Returns:
            Age.
        """
        try:
            return self._body.age
        except Exception as e:
            raise AgentError(f"Failed to get age: {e}")
    
    def setAge(self, age: float) -> None:
        """Set age.
        
        Args:
            age: Age.
        """
        try:
            self._body.age = age
        except Exception as e:
            raise AgentError(f"Failed to set age: {e}")
    
    def isMoving(self) -> bool:
        """Check if agent is moving.
        
        Returns:
            True if agent is moving, False otherwise.
        """
        try:
            return self._body.isMoving
        except Exception as e:
            raise AgentError(f"Failed to check if agent is moving: {e}")
    
    def setMoving(self, moving: bool) -> None:
        """Set moving status.
        
        Args:
            moving: Moving status.
        """
        try:
            self._body.isMoving = moving
        except Exception as e:
            raise AgentError(f"Failed to set moving status: {e}")
    
    def isTurning(self) -> bool:
        """Check if agent is turning.
        
        Returns:
            True if agent is turning, False otherwise.
        """
        try:
            return self._body.isTurning
        except Exception as e:
            raise AgentError(f"Failed to check if agent is turning: {e}")
    
    def setTurning(self, turning: bool) -> None:
        """Set turning status.
        
        Args:
            turning: Turning status.
        """
        try:
            self._body.isTurning = turning
        except Exception as e:
            raise AgentError(f"Failed to set turning status: {e}")
    
    def getLastActionTime(self) -> float:
        """Get last action time.
        
        Returns:
            Last action time.
        """
        try:
            return self._body.lastActionTime
        except Exception as e:
            raise AgentError(f"Failed to get last action time: {e}")
    
    def setLastActionTime(self, time: float) -> None:
        """Set last action time.
        
        Args:
            time: Last action time.
        """
        try:
            self._body.lastActionTime = time
        except Exception as e:
            raise AgentError(f"Failed to set last action time: {e}")
    
    def reset(self) -> None:
        """Reset agent body to initial state.
        
        Raises:
            AgentError: If reset fails.
        """
        try:
            self._body.reset()
        except Exception as e:
            raise AgentError(f"Failed to reset agent body: {e}")
    
    # Pythonic properties
    
    @property
    def x(self) -> float:
        """X coordinate."""
        return self.getX()
    
    @x.setter
    def x(self, value: float) -> None:
        self.setX(value)
    
    @property
    def y(self) -> float:
        """Y coordinate."""
        return self.getY()
    
    @y.setter
    def y(self, value: float) -> None:
        self.setY(value)
    
    @property
    def orientation(self) -> float:
        """Orientation."""
        return self.getOrientation()
    
    @orientation.setter
    def orientation(self, value: float) -> None:
        self.setOrientation(value)
    
    @property
    def velocity_x(self) -> float:
        """X velocity."""
        return self.getVelocityX()
    
    @velocity_x.setter
    def velocity_x(self, value: float) -> None:
        self.setVelocityX(value)
    
    @property
    def velocity_y(self) -> float:
        """Y velocity."""
        return self.getVelocityY()
    
    @velocity_y.setter
    def velocity_y(self, value: float) -> None:
        self.setVelocityY(value)
    
    @property
    def angular_velocity(self) -> float:
        """Angular velocity."""
        return self.getAngularVelocity()
    
    @angular_velocity.setter
    def angular_velocity(self, value: float) -> None:
        self.setAngularVelocity(value)
    
    @property
    def energy(self) -> float:
        """Energy."""
        return self.getEnergy()
    
    @energy.setter
    def energy(self, value: float) -> None:
        self.setEnergy(value)
    
    @property
    def health(self) -> float:
        """Health."""
        return self.getHealth()
    
    @health.setter
    def health(self, value: float) -> None:
        self.setHealth(value)
    
    @property
    def age(self) -> float:
        """Age."""
        return self.getAge()
    
    @age.setter
    def age(self, value: float) -> None:
        self.setAge(value)
    
    @property
    def is_moving(self) -> bool:
        """Moving status."""
        return self.isMoving()
    
    @is_moving.setter
    def is_moving(self, value: bool) -> None:
        self.setMoving(value)
    
    @property
    def is_turning(self) -> bool:
        """Turning status."""
        return self.isTurning()
    
    @is_turning.setter
    def is_turning(self, value: bool) -> None:
        self.setTurning(value)
    
    @property
    def last_action_time(self) -> float:
        """Last action time."""
        return self.getLastActionTime()
    
    @last_action_time.setter
    def last_action_time(self, value: float) -> None:
        self.setLastActionTime(value)
    
    def __repr__(self) -> str:
        return f"EnhancedAgentBody(x={self.x}, y={self.y}, energy={self.energy}, health={self.health})"
class AgentBody(EnhancedAgentBody):
    """AgentBody class for NLM.
    
    This class provides agent body state for the NLM system.
    The agent body represents the physical state of an agent in the world,
    including position, orientation, velocity, energy, health, and age.
    
    Example:
        >>> # Create agent body
        >>> body = AgentBody()
        >>> 
        >>> # Set position
        >>> body.x = 10.0
        >>> body.y = 10.0
        >>> 
        >>> # Access state
        >>> print(f"Position: ({body.x}, {body.y})")
        >>> print(f"Energy: {body.energy}")
        >>> print(f"Health: {body.health}")
    """
    
    def __init__(self):
        """Initialize agent body."""
        super().__init__()


# Custom exception for action errors
class ActionError(NLMError):
    """Error in action processing."""
    pass

# Custom exception for world errors
class WorldError(NLMError):
    """Error in world processing."""
    pass

# Custom exception for agent errors
class AgentError(NLMError):
    """Error in agent processing."""
    pass

# Custom exception for sensory errors
class SensoryError(NLMError):
    """Error in sensory processing."""
    pass


# Type aliases for better documentation
ActionParameters = List[float]
WorldPosition = Tuple[float, float]
WorldCoordinates = Tuple[float, float]

__all__ = [
    # Main classes
    "Action", "EnhancedAction",
    "WorldObject", "EnhancedWorldObject",
    "ActionResult", "EnhancedActionResult",
    "AgentBody", "EnhancedAgentBody",
    
    # Exceptions
    "ActionError", "WorldError", "AgentError", "SensoryError",
    
    # Type aliases
    "ActionParameters", "WorldPosition", "WorldCoordinates",
]


def createAction(action_type: Any, parameters: Optional[List[float]] = None) -> Action:
    """Create action from type and parameters.
    
    Args:
        action_type: Action type.
        parameters: Action parameters.
        
    Returns:
        Action object.
    """
    return Action(action_type, parameters)


def createWorldObject(x: float, y: float, obj_type: Any, value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create world object from parameters.
    
    Args:
        x: X coordinate.
        y: Y coordinate.
        obj_type: Object type.
        value: Object value.
        radius: Object radius.
        
    Returns:
        WorldObject object.
    """
    return WorldObject(x, y, obj_type, value, radius)


def createAgentBody() -> AgentBody:
    """Create agent body.
    
    Returns:
        AgentBody object.
    """
    return AgentBody()


def createActionResult(reward: float = 0.0, success: bool = False, message: str = "") -> ActionResult:
    """Create action result from parameters.
    
    Args:
        reward: Reward value.
        success: Success status.
        message: Message.
        
    Returns:
        ActionResult object.
    """
    return ActionResult(reward, success, message)
