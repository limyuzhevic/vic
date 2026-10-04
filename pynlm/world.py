"""
NLM World classes
"""

from typing import List, Optional, Any
from .sensory import SensoryPercept

class WorldError(Exception):
    """Raised when world operations fail."""
    pass
class WorldObjectType:
    """World object type enumeration."""
    
    EMPTY = "Empty"
    RESOURCE = "Resource"
    HAZARD = "Hazard"
    WALL = "Wall"
    MARKER = "Marker"
class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: str = WorldObjectType.EMPTY, 
                 value: float = 0.0, radius: float = 0.5):
        """Create a world object.
        
        Args:
            x: X coordinate
            y: Y coordinate
            obj_type: Type of object
            value: Object value (e.g., resource amount)
            radius: Object radius
        """
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def __repr__(self) -> str:
        return f"WorldObject(x={self.x}, y={self.y}, type={self.type})"
class AgentBody:
    """Agent body state."""
    
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 1.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def reset(self) -> None:
        """Reset agent body state."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 1.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
class ActionResult:
    """Action result from world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """Create an action result.
        
        Args:
            reward: Reward received
            success: Whether action succeeded
            message: Result message
        """
        self.reward = reward
        self.success = success
        self.message = message
class SimpleWorld:
    """Simple 2D world for NLM simulation.
    
    Provides a virtual environment where agents can interact,
    explore, and learn from their experiences.
    """
    
    def __init__(self):
        """Create a simple world."""
        self.width = 20
        self.height = 20
        self.vision_width = 8
        self.vision_height = 8
        
        # Agent state
        self.agent_x = 10.0
        self.agent_y = 10.0
        self.agent_orientation = 0.0
        
        # World objects
        self.objects: List[WorldObject] = []
        
        # World state
        self.simulation_time = 0.0
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.random_seed = 42
        
        print("SimpleWorld created")
    
    def configure(self, width: int, height: int, 
                  vision_width: int, vision_height: int) -> None:
        """Configure world dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        print(f"SimpleWorld configured: {width}x{height}, vision: {vision_width}x{vision_height}")
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.agent_orientation = 0.0
        self.objects.clear()
        self.simulation_time = 0.0
        print("SimpleWorld reset")
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self.agent_x = x
        self.agent_y = y
        print(f"Agent start set to ({x}, {y})")
    
    def update(self, timestep: float) -> None:
        """Update world by timestep seconds.
        
        Args:
            timestep: Time step to update
        """
        self.simulation_time += timestep
        
        # Update agent age
        # For demonstration, just print update
        print(f"SimpleWorld updated: timestep={timestep}, simulation_time={self.simulation_time}")
    
    def apply_motor_command(self, command: Any, current_time: float) -> None:
        """Apply motor command to world.
        
        Args:
            command: Motor command to apply
            current_time: Current simulation time
        """
        print(f"SimpleWorld applying motor command: {command} at time {current_time}")
        
        # For demonstration, just update agent position based on command
        if command and hasattr(command, 'type'):
            if command.type == "MoveForward":
                self.agent_x += 1.0
            elif command.type == "TurnLeft":
                self.agent_orientation -= 0.1
            elif command.type == "TurnRight":
                self.agent_orientation += 0.1
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get sensory percept from world.
        
        Returns:
            Sensory percept instance
        """
        percept = SensoryPercept()
        
        # Vision (simplified: just return a uniform field)
        vision = []
        for _ in range(self.vision_width * self.vision_height):
            vision.append(0.5)  # Mid-gray
        
        percept.setVision(vision)
        
        # Touch (simplified)
        touch = []
        for _ in range(8):
            touch.append(0.0)
        
        percept.setTouch(touch)
        
        # Internal signals (simplified)
        internal = []
        for _ in range(4):
            internal.append(0.5)
        
        percept.setInternal(internal)
        
        # Proprioception (simplified)
        proprioception = []
        for _ in range(6):
            proprioception.append(0.5)
        
        percept.setProprioception(proprioception)
        
        # Audio (simplified)
        audio = []
        for _ in range(0):  # No audio for now
            audio.append(0.0)
        
        percept.setAudio(audio)
        
        return percept
    
    def get_agent_body(self) -> AgentBody:
        """Get agent body state.
        
        Returns:
            Agent body instance
        """
        body = AgentBody()
        body.x = self.agent_x
        body.y = self.agent_y
        body.orientation = self.agent_orientation
        body.energy = self.max_energy  # Simplified
        return body
    
    def add_object(self, obj: WorldObject) -> None:
        """Add world object.
        
        Args:
            obj: World object to add
        """
        self.objects.append(obj)
        print(f"Added object at ({obj.x}, {obj.y}): {obj.type}")
    
    def remove_object(self, x: float, y: float) -> None:
        """Remove world object.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        original_count = len(self.objects)
        self.objects = [obj for obj in self.objects if not (abs(obj.x - x) < 0.5 and abs(obj.y - y) < 0.5)]
        removed_count = original_count - len(self.objects)
        if removed_count > 0:
            print(f"Removed {removed_count} object(s) at ({x}, {y})")
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not blocked by wall).
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid
        """
        # Check if within world bounds
        if x < 0 or x >= self.width or y < 0 or y >= self.height:
            return False
        
        # Check for walls (simple implementation)
        for obj in self.objects:
            if obj.type == WorldObjectType.WALL and abs(obj.x - x) < 0.5 and abs(obj.y - y) < 0.5:
                return False
        
        return True
    
    def get_width(self) -> int:
        """Get world width.
        
        Returns:
            World width
        """
        return self.width
    
    def get_height(self) -> int:
        """Get world height.
        
        Returns:
            World height
        """
        return self.height
    
    def get_max_energy(self) -> float:
        """Get maximum energy.
        
        Returns:
            Maximum energy
        """
        return self.max_energy
    
    def set_max_energy(self, e: float) -> None:
        """Set maximum energy.
        
        Args:
            e: Maximum energy
        """
        self.max_energy = e
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            Energy decay rate
        """
        return self.energy_decay_rate
    
    def set_energy_decay_rate(self, r: float) -> None:
        """Set energy decay rate.
        
        Args:
            r: Energy decay rate
        """
        self.energy_decay_rate = r
    
    def get_simulation_time(self) -> float:
        """Get simulation time.
        
        Returns:
            Current simulation time
        """
        return self.simulation_time
    
    def set_random_seed(self, seed: int) -> None:
        """Set random seed.
        
        Args:
            seed: Random seed
        """
        self.random_seed = seed
    
    def get_random_seed(self) -> int:
        """Get random seed.
        
        Returns:
            Random seed
        """
        return self.random_seed