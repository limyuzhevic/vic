# World implementation for NLM Python API

"""
Simple 2D world for NLM simulation

Provides a Pythonic interface to the NLM world simulation,
with type hints, better error handling, and convenient methods.
"""

from typing import List, Any, Optional, Tuple
from dataclasses import dataclass
import math

# Import the C++ binding and type definitions
from pynlm import WorldObject as CppWorldObject
from pynlm import AgentBody as CppAgentBody
from pynlm import ActionResult as CppActionResult
from pynlm import SensoryPercept as CppSensoryPercept
from pynlm.types import Timestamp, Vision, Audio, InternalSignals, SensoryInput, ActionType

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class WorldError(NLMError):
    """Raised when world operations fail"""
    pass

class SimpleWorld:
    """
    Simple 2D world for NLM simulation
    
    Represents a virtual environment where the NLM brain interacts,
    providing sensory input and executing motor actions.
    
    The world is a 2D grid-based environment with objects, resources,
    hazards, and an agent that navigates through it.
    
    Examples:
        >>> world = SimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        >>> world.reset()
        >>> world.setAgentStart(10, 10)
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)  # Update world
        ...     percept = world.getSensoryPercept()  # What does agent see?
        ...     # Agent processes percept and produces action
    """
    
    def __init__(self):
        """
        Initialize world.
        
        Examples:
            >>> world = SimpleWorld()  # Create new world
        """
        # Note: In actual implementation, this would wrap a C++ world object
        self._configured = False
        self._reset = False
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """
        Configure world dimensions and agent vision.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent vision width in cells
            vision_height: Agent vision height in cells
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        """
        # Validate parameters
        if width <= 0 or height <= 0:
            raise WorldError(f"Invalid world dimensions: {width}x{height}")
        if vision_width <= 0 or vision_height <= 0:
            raise WorldError(f"Invalid vision dimensions: {vision_width}x{vision_height}")
        
        # Note: In actual implementation, this would call C++ configure method
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        self._configured = True
    
    def reset(self) -> None:
        """
        Reset world to initial state.
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()  # Reset to start state
        """
        # Note: In actual implementation, this would call C++ reset method
        self._reset = True
        self._simulation_time = 0.0
    
    def set_agent_start(self, x: int, y: int) -> None:
        """
        Set agent starting position.
        
        Args:
            x: Starting X coordinate
            y: Starting Y coordinate
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20)
            >>> world.set_agent_start(10, 10)  # Start at center
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        
        if not (0 <= x < self._width and 0 <= y < self._height):
            raise WorldError(f"Invalid agent start position: ({x}, {y})")
        
        # Note: In actual implementation, this would call C++ setAgentStart method
        self._agent_x = x
        self._agent_y = y
        self._agent_orientation = 0.0  # Facing right (0 degrees)
        self._agent_velocity_x = 0.0
        self._agent_velocity_y = 0.0
        self._agent_angular_velocity = 0.0
        self._agent_energy = 100.0
        self._agent_health = 100.0
        self._agent_age = 0.0
        self._agent_is_moving = False
        self._agent_is_turning = False
    
    def update(self, timestep: float) -> None:
        """
        Update world state by timestep.
        
        Args:
            timestep: Time step in seconds
            
        Raises:
            WorldError: If world update fails
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> world.update(0.1)  # Update by 100ms
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        if timestep <= 0:
            raise WorldError(f"Invalid timestep: {timestep}")
        
        try:
            # Note: In actual implementation, this would call C++ update method
            self._simulation_time += timestep
            self._agent_age += timestep
            
            # Update agent energy
            self._agent_energy = max(0.0, self._agent_energy - timestep * 0.5)
            
            # Keep agent within bounds
            self._agent_x = max(0, min(self._agent_x, self._width - 1))
            self._agent_y = max(0, min(self._agent_y, self._height - 1))
            
        except Exception as e:
            raise WorldError(f"Failed to update world: {e}")
    
    def apply_motor_command(self, action: Any, current_time: Timestamp) -> ActionResult:
        """
        Apply motor command to world and execute action.
        
        Args:
            action: Motor command/action to execute
            current_time: Current simulation time
            
        Returns:
            Action result containing reward, success status, and message
            
        Raises:
            WorldError: If action execution fails
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> world.set_agent_start(5, 5)
            >>> action = Action(ActionType.MoveForward)
            >>> result = world.applyMotorCommand(action, 0.0)
            >>> print(f"Reward: {result.reward}")
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        try:
            # Note: In actual implementation, this would call C++ applyMotorCommand method
            result = ActionResult()
            
            # Parse action and update agent state
            action_type = getattr(action, 'type', None)
            
            if action_type == ActionType.MoveForward:
                # Move forward
                angle_rad = math.radians(self._agent_orientation)
                self._agent_x += math.cos(angle_rad) * 0.5
                self._agent_y += math.sin(angle_rad) * 0.5
                result.reward = 1.0
                result.success = True
                result.message = "Moved forward"
                
            elif action_type == ActionType.MoveBackward:
                # Move backward
                angle_rad = math.radians(self._agent_orientation)
                self._agent_x -= math.cos(angle_rad) * 0.5
                self._agent_y -= math.sin(angle_rad) * 0.5
                result.reward = 0.5
                result.success = True
                result.message = "Moved backward"
                
            elif action_type == ActionType.TurnLeft:
                # Turn left
                self._agent_orientation -= 30.0  # 30 degrees left
                result.reward = 0.1
                result.success = True
                result.message = "Turned left"
                
            elif action_type == ActionType.TurnRight:
                # Turn right
                self._agent_orientation += 30.0  # 30 degrees right
                result.reward = 0.1
                result.success = True
                result.message = "Turned right"
                
            elif action_type == ActionType.Interact:
                # Interaction action
                result.reward = 2.0
                result.success = True
                result.message = "Interacted with object"
                
            elif action_type == ActionType.Wait:
                # Wait action
                result.reward = 0.0
                result.success = True
                result.message = "Waited"
                
            else:
                result.reward = 0.0
                result.success = False
                result.message = f"Unknown action: {action_type}"
            
            # Update agent movement state
            if action_type in [ActionType.MoveForward, ActionType.MoveBackward]:
                self._agent_is_moving = True
                self._agent_velocity_x = math.cos(math.radians(self._agent_orientation)) * 0.5
                self._agent_velocity_y = math.sin(math.radians(self._agent_orientation)) * 0.5
            else:
                self._agent_is_moving = False
                self._agent_velocity_x = 0.0
                self._agent_velocity_y = 0.0
                
            if action_type in [ActionType.TurnLeft, ActionType.TurnRight]:
                self._agent_is_turning = True
                self._agent_angular_velocity = 0.1 if action_type == ActionType.TurnLeft else -0.1
            else:
                self._agent_is_turning = False
                self._agent_angular_velocity = 0.0
            
            return result
            
        except Exception as e:
            raise WorldError(f"Failed to apply motor command: {e}")
    
    def get_sensory_percept(self) -> SensoryPercept:
        """
        Get sensory percept that agent sees.
        
        Returns:
            Sensory percept containing vision, touch, internal signals, etc.
            
        Raises:
            WorldError: If sensory percept cannot be retrieved
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
            >>> world.reset()
            >>> world.set_agent_start(5, 5)
            >>> percept = world.getSensoryPercept()
            >>> vision = percept.getVision()
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        try:
            # Note: In actual implementation, this would call C++ getSensoryPercept method
            percept = SensoryPercept()
            
            # Create vision data (simple example)
            vision_data = []
            for y in range(self._vision_height):
                row = []
                for x in range(self._vision_width):
                    # Check if there's an object at this vision position
                    world_x = self._agent_x + x - self._vision_width // 2
                    world_y = self._agent_y + y - self._vision_height // 2
                    
                    # Simple vision: check what's at this position
                    if 0 <= world_x < self._width and 0 <= world_y < self._height:
                        if world_x == self._agent_x and world_y == self._agent_y:
                            row.append([1.0, 0.0, 0.0])  # Red = agent
                        else:
                            row.append([0.0, 1.0, 0.0])  # Green = empty
                    else:
                        row.append([0.0, 0.0, 0.0])  # Black = out of bounds
                    
                    vision_data.append(row)
            
            # Set vision data
            if hasattr(percept, 'setVision'):
                percept.setVision(vision_data)
            
            return percept
            
        except Exception as e:
            raise WorldError(f"Failed to get sensory percept: {e}")
    
    def get_agent_body(self) -> AgentBody:
        """
        Get current agent body state.
        
        Returns:
            Agent body state object
            
        Raises:
            WorldError: If agent body cannot be retrieved
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> world.set_agent_start(5, 5)
            >>> body = world.get_agent_body()
            >>> print(f"Agent position: ({body.x}, {body.y})")
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        try:
            # Note: In actual implementation, this would call C++ getAgentBody method
            body = AgentBody()
            body.x = self._agent_x
            body.y = self._agent_y
            body.orientation = self._agent_orientation
            body.velocityX = self._agent_velocity_x
            body.velocityY = self._agent_velocity_y
            body.angularVelocity = self._agent_angular_velocity
            body.energy = self._agent_energy
            body.health = self._agent_health
            body.age = self._agent_age
            body.isMoving = self._agent_is_moving
            body.isTurning = self._agent_is_turning
            body.lastActionTime = self._simulation_time
            
            return body
            
        except Exception as e:
            raise WorldError(f"Failed to get agent body: {e}")
    
    def add_object(self, obj: Any) -> None:
        """
        Add an object to the world.
        
        Args:
            obj: World object to add
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> obj = WorldObject(x=3, y=3, type=WorldObjectType.Resource, value=10.0)
            >>> world.addObject(obj)
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        try:
            # Note: In actual implementation, this would call C++ addObject method
            self._objects = getattr(self, '_objects', [])
            self._objects.append(obj)
            
        except Exception as e:
            raise WorldError(f"Failed to add object: {e}")
    
    def remove_object(self, x: int, y: int) -> None:
        """
        Remove object at given position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> world.removeObject(3, 3)
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        try:
            # Note: In actual implementation, this would call C++ removeObject method
            self._objects = getattr(self, '_objects', [])
            self._objects = [obj for obj in self._objects if not (obj.x == x and obj.y == y)]
            
        except Exception as e:
            raise WorldError(f"Failed to remove object: {e}")
    
    def is_valid_position(self, x: int, y: int) -> bool:
        """
        Check if position is valid (within bounds and not blocked).
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid, False otherwise
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> if world.isValidPosition(5, 5):
            ...     print("Position is valid")
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        if not self._reset:
            raise WorldError("World not reset. Call reset() first.")
        
        # Check bounds
        if not (0 <= x < self._width and 0 <= y < self._height):
            return False
        
        # Check for blocking objects (walls, hazards)
        # Note: In actual implementation, this would check world objects
        # For now, just check bounds
        return True
    
    def get_width(self) -> int:
        """
        Get world width.
        
        Returns:
            World width in cells
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20)
            >>> width = world.getWidth()
            >>> print(f"World width: {width}")
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        return self._width
    
    def get_height(self) -> int:
        """
        Get world height.
        
        Returns:
            World height in cells
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20)
            >>> height = world.getHeight()
            >>> print(f"World height: {height}")
        """
        if not self._configured:
            raise WorldError("World not configured. Call configure() first.")
        return self._height
    
    def get_max_energy(self) -> float:
        """
        Get maximum agent energy.
        
        Returns:
            Maximum energy value
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> max_energy = world.getMaxEnergy()
        """
        # Note: In actual implementation, this would get from C++
        return 100.0
    
    def set_max_energy(self, energy: float) -> None:
        """
        Set maximum agent energy.
        
        Args:
            energy: Maximum energy value
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.setMaxEnergy(200.0)
        """
        # Note: In actual implementation, this would set in C++
        pass
    
    def get_energy_decay_rate(self) -> float:
        """
        Get energy decay rate.
        
        Returns:
            Energy decay rate per second
            
        Examples:
            >>> world = SimpleWorld()
            >>> decay_rate = world.getEnergyDecayRate()
            >>> print(f"Energy decay rate: {decay_rate}/s")
        """
        # Note: In actual implementation, this would get from C++
        return 0.5
    
    def set_energy_decay_rate(self, rate: float) -> None:
        """
        Set energy decay rate.
        
        Args:
            rate: Energy decay rate per second
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.setEnergyDecayRate(1.0)
        """
        # Note: In actual implementation, this would set in C++
        pass
    
    def get_simulation_time(self) -> float:
        """
        Get current simulation time.
        
        Returns:
            Current simulation time in seconds
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=10, height=10)
            >>> world.reset()
            >>> time = world.getSimulationTime()
            >>> print(f"Simulation time: {time}s")
        """
        return self._simulation_time
    
    def set_random_seed(self, seed: int) -> None:
        """
        Set random seed for world.
        
        Args:
            seed: Random seed value
            
        Examples:
            >>> world = SimpleWorld()
            >>> world.setRandomSeed(12345)
        """
        # Note: In actual implementation, this would set in C++
        self._random_seed = seed
    
    def get_random_seed(self) -> int:
        """
        Get random seed.
        
        Returns:
            Random seed value
            
        Examples:
            >>> world = SimpleWorld()
            >>> seed = world.getRandomSeed()
        """
        # Note: In actual implementation, this would get from C++
        return getattr(self, '_random_seed', 0)
    
    def __repr__(self) -> str:
        """String representation of world"""
        if not self._configured:
            return "<World not configured>"
        elif not self._reset:
            return f"<World {self._width}x{self._height} not reset>"
        else:
            return f"<World {self._width}x{self._height} reset at ({self._agent_x:.1f}, {self._agent_y:.1f})>"
    
    def __str__(self) -> str:
        """String representation of world"""
        return self.__repr__()

# Factory functions
def create_simple_world() -> SimpleWorld:
    """
    Create a simple world.
    
    Returns:
        New SimpleWorld instance
        
    Examples:
        >>> world = createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    """
    return SimpleWorld()

# ActionResult class
class ActionResult:
    """
    Result from applying a motor command to the world.
    
    Contains reward, success status, and message about the action execution.
    
    Attributes:
        reward: Reward value from action (higher is better)
        success: Whether action was successful
        message: Description of action result
    """
    
    def __init__(self):
        self.reward = 0.0
        self.success = False
        self.message = ""
    
    def __repr__(self) -> str:
        return f"<ActionResult reward={self.reward}, success={self.success}, message='{self.message}'>"

# AgentBody class (simplified version)
class AgentBody:
    """
    Agent body state
    
    Contains position, orientation, velocity, and physiological state.
    
    Attributes:
        x: X coordinate
        y: Y coordinate
        orientation: Orientation in degrees (0 = right, 90 = up)
        velocityX: X velocity
        velocityY: Y velocity
        angularVelocity: Angular velocity (degrees per second)
        energy: Current energy level
        health: Current health level
        age: Agent age in seconds
        isMoving: Whether agent is currently moving
        isTurning: Whether agent is currently turning
        lastActionTime: Time of last action
    """
    
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 0.0
        self.health = 0.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def __repr__(self) -> str:
        return f"<AgentBody pos=({self.x:.1f}, {self.y:.1f}), dir={self.orientation:.1f}°, energy={self.energy:.1f}>"

# SensoryPercept class (simplified version)
class SensoryPercept:
    """
    Sensory percept data from world
    
    Contains vision, touch, internal signals, proprioception, and audio.
    """
    
    def __init__(self):
        self._vision = []
        self._touch = []
        self._internal = []
        self._proprioception = []
        self._audio = []
        self._timestamp = 0.0
    
    def get_vision(self) -> List[List[List[float]]]:
        """Get vision data"""
        return self._vision
    
    def set_vision(self, vision: List[List[List[float]]]) -> None:
        """Set vision data"""
        self._vision = vision
    
    def get_vision_width(self) -> int:
        """Get vision width"""
        return len(self._vision[0]) if self._vision else 0
    
    def get_vision_height(self) -> int:
        """Get vision height"""
        return len(self._vision) if self._vision else 0
    
    def get_touch(self) -> List[float]:
        """Get touch data"""
        return self._touch
    
    def set_touch(self, touch: List[float]) -> None:
        """Set touch data"""
        self._touch = touch
    
    def get_internal(self) -> List[float]:
        """Get internal signals"""
        return self._internal
    
    def set_internal(self, internal: List[float]) -> None:
        """Set internal signals"""
        self._internal = internal
    
    def get_proprioception(self) -> List[float]:
        """Get proprioception data"""
        return self._proprioception
    
    def set_proprioception(self, proprioception: List[float]) -> None:
        """Set proprioception data"""
        self._proprioception = proprioception
    
    def get_audio(self) -> List[float]:
        """Get audio data"""
        return self._audio
    
    def set_audio(self, audio: List[float]) -> None:
        """Set audio data"""
        self._audio = audio
    
    def get_all_signals(self) -> Dict[str, Any]:
        """Get all sensory signals"""
        return {
            'vision': self._vision,
            'touch': self._touch,
            'internal': self._internal,
            'proprioception': self._proprioception,
            'audio': self._audio,
            'timestamp': self._timestamp
        }
    
    def get_timestamp(self) -> float:
        """Get timestamp"""
        return self._timestamp
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set timestamp"""
        self._timestamp = timestamp
    
    def __repr__(self) -> str:
        return f"<SensoryPercept vision={self.get_vision_width()}x{self.get_vision_height()}, touch={len(self._touch)} signals>"