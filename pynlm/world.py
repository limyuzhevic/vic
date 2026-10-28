# World simulation interface for NLM
import numpy as np
from typing import Optional, List, Tuple, Any
from enum import Enum

from .sensory import SensoryInput
from .agent import AgentBody

class WorldObjectType(Enum):
    """Types of objects in the world."""
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

class WorldObject:
    """Object in the simulation world."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: WorldObjectType = WorldObjectType.EMPTY,
                 value: float = 0.0, radius: float = 0.5):
        """Create world object."""
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def distance_to(self, other: 'WorldObject') -> float:
        """Calculate distance to another object."""
        return np.sqrt((self.x - other.x)**2 + (self.y - other.y)**2)
    
    def contains_point(self, x: float, y: float) -> bool:
        """Check if point is inside this object."""
        return self.distance_to(WorldObject(x, y)) <= self.radius
    
    def __repr__(self) -> str:
        return f"<WorldObject type={self.type.name} x={self.x:.1f} y={self.y:.1f}>"

class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        """Initialize empty world."""
        self.width = 20.0
        self.height = 20.0
        self.vision_width = 8
        self.vision_height = 8
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.simulation_time = 0.0
        self.random_seed = 42
        
        # Agent state
        self.agent_x = 10.0
        self.agent_y = 10.0
        self.agent_orientation = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_angular_velocity = 0.0
        self.agent_energy = 100.0
        self.agent_health = 100.0
        self.agent_age = 0.0
        
        # World objects
        self.objects: List[WorldObject] = []
        
        # Sensory input cache
        self._percept = None
        
        # Vision grid
        self._vision_grid = None
    
    def configure(self, width: int, height: int, 
                  vision_width: int, vision_height: int) -> None:
        """Configure world dimensions and vision."""
        self.width = float(width)
        self.height = float(height)
        self.vision_width = vision_width
        self.vision_height = vision_height
        self._vision_grid = np.zeros((vision_height, vision_width))
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self.simulation_time = 0.0
        self.agent_x = self.width / 2.0
        self.agent_y = self.height / 2.0
        self.agent_orientation = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_angular_velocity = 0.0
        self.agent_energy = self.max_energy
        self.agent_health = 100.0
        self.agent_age = 0.0
        
        self.objects.clear()
        self._percept = None
        self._vision_grid.fill(0.0)
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self.agent_x = max(0.0, min(x, self.width))
        self.agent_y = max(0.0, min(y, self.height))
    
    def update(self, timestep: float) -> None:
        """Update world state by timestep."""
        # Update simulation time
        self.simulation_time += timestep
        self.agent_age += timestep
        
        # Update agent physics (simplified)
        self.agent_x += self.agent_velocity_x * timestep
        self.agent_y += self.agent_velocity_y * timestep
        
        # Keep agent within bounds
        self.agent_x = max(0.0, min(self.agent_x, self.width))
        self.agent_y = max(0.0, min(self.agent_y, self.height))
        
        # Decay energy
        self.agent_energy *= (1.0 - self.energy_decay_rate * timestep)
        
        # Update vision grid
        self._update_vision()
        
        # Generate percept
        self._percept = self._create_percept()
    
    def applyMotorCommand(self, action, current_time: float) -> None:
        """Apply motor command to world."""
        # This is a simplified implementation
        # In real usage, this would be replaced by actual action handling
        
        # For now, just store the action
        self._last_action = action
        self._last_action_time = current_time
        
        # Simple movement based on action type
        action_type = getattr(action, 'type', None)
        if action_type:
            movement_speed = 1.0
            if action_type.name == 'MoveForward':
                self.agent_velocity_x += movement_speed * timestep
            elif action_type.name == 'MoveBackward':
                self.agent_velocity_x -= movement_speed * timestep
            elif action_type.name == 'TurnLeft':
                self.agent_orientation -= np.pi * 0.1
            elif action_type.name == 'TurnRight':
                self.agent_orientation += np.pi * 0.1
    
    def getSensoryPercept(self) -> SensoryInput:
        """Get sensory percept of current world state."""
        if self._percept is None:
            self._percept = self._create_percept()
        return self._percept
    
    def getAgentBody(self) -> AgentBody:
        """Get current agent body state."""
        body = AgentBody()
        body.x = self.agent_x
        body.y = self.agent_y
        body.orientation = self.agent_orientation
        body.velocityX = self.agent_velocity_x
        body.velocityY = self.agent_velocity_y
        body.angularVelocity = self.agent_angular_velocity
        body.energy = self.agent_energy
        body.health = self.agent_health
        body.age = self.agent_age
        body.isMoving = (abs(self.agent_velocity_x) > 0.01 or abs(self.agent_velocity_y) > 0.01)
        body.isTurning = (abs(self.agent_angular_velocity) > 0.01)
        body.lastActionTime = self._last_action_time if hasattr(self, '_last_action_time') else 0.0
        return body
    
    def addObject(self, obj: WorldObject) -> None:
        """Add object to world."""
        self.objects.append(obj)
    
    def removeObject(self, x: float, y: float) -> bool:
        """Remove object at given position."""
        for i, obj in enumerate(self.objects):
            if obj.contains_point(x, y):
                del self.objects[i]
                return True
        return False
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid (within bounds and not occupied)."""
        if not (0.0 <= x <= self.width and 0.0 <= y <= self.height):
            return False
        
        for obj in self.objects:
            if obj.contains_point(x, y):
                return False
        
        return True
    
    # Property getters
    def getWidth(self) -> float:
        """Get world width."""
        return self.width
    
    def getHeight(self) -> float:
        """Get world height."""
        return self.height
    
    def getMaxEnergy(self) -> float:
        """Get maximum agent energy."""
        return self.max_energy
    
    def setMaxEnergy(self, e: float) -> None:
        """Set maximum agent energy."""
        self.max_energy = e
        self.agent_energy = e
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate."""
        return self.energy_decay_rate
    
    def setEnergyDecayRate(self, r: float) -> None:
        """Set energy decay rate."""
        self.energy_decay_rate = r
    
    def getSimulationTime(self) -> float:
        """Get current simulation time."""
        return self.simulation_time
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed."""
        self.random_seed = seed
        np.random.seed(seed)
    
    def getRandomSeed(self) -> int:
        """Get random seed."""
        return self.random_seed
    
    def _update_vision(self) -> None:
        """Update agent's vision based on position and orientation."""
        # Simplified vision implementation
        vision_size = min(self.vision_width, self.vision_height)
        half_vision = vision_size // 2
        
        # Get objects in vision range
        visible_objects = []
        for obj in self.objects:
            if obj.active:
                dist = np.sqrt((obj.x - self.agent_x)**2 + (obj.y - self.agent_y)**2)
                if dist <= half_vision * 0.5:  # Vision range
                    visible_objects.append((dist, obj))
        
        # Sort by distance (closest first)
        visible_objects.sort(key=lambda x: x[0])
        
        # Build vision grid
        self._vision_grid.fill(0.0)
        for dist, obj in visible_objects:
            # Simple ray casting for vision
            angle_to_obj = np.arctan2(obj.y - self.agent_y, obj.x - self.agent_x)
            rel_angle = angle_to_obj - self.agent_orientation
            
            # Normalize angle to [-pi, pi]
            while rel_angle > np.pi:
                rel_angle -= 2 * np.pi
            while rel_angle < -np.pi:
                rel_angle += 2 * np.pi
            
            # Map to vision grid cell
            grid_angle = (rel_angle + np.pi) / (2 * np.pi) * vision_size
            grid_x = int(grid_angle * self.vision_width / vision_size)
            grid_y = int(dist / (half_vision * 0.5) * self.vision_height / 2)
            
            if 0 <= grid_x < self.vision_width and 0 <= grid_y < self.vision_height:
                self._vision_grid[grid_y, grid_x] = 1.0
    
    def _create_percept(self) -> SensoryInput:
        """Create sensory percept from current world state."""
        from .sensory import Vision
        
        percept = SensoryInput()
        
        # Create vision percept from grid
        vision_data = []
        for y in range(self.vision_height):
            row = []
            for x in range(self.vision_width):
                row.append(float(self._vision_grid[y, x]))
            vision_data.append(row)
        
        vision = Vision(self.vision_width, self.vision_height)
        vision.setData(np.array(vision_data).flatten())
        percept.setVision(vision)
        
        # Add internal signals
        internal = []
        internal.append(self.agent_energy / self.max_energy)  # Normalized energy
        internal.append(self.agent_health / 100.0)  # Normalized health
        internal.append(self.agent_age)  # Age
        internal.append(self.simulation_time)  # Simulation time
        
        from .sensory import InternalSignals
        internal_signals = InternalSignals()
        for val in internal:
            internal_signals.addSignal(val)
        percept.setInternal(internal_signals)
        
        # Add proprioception
        proprioception = []
        proprioception.append(self.agent_orientation)  # Orientation
        proprioception.append(self.agent_velocity_x)  # Velocity X
        proprioception.append(self.agent_velocity_y)  # Velocity Y
        proprioception.append(self.agent_angular_velocity)  # Angular velocity
        
        percept.setProprioception(np.array(proprioception))
        
        return percept
    
    def __repr__(self) -> str:
        """String representation of world."""
        return f"<SimpleWorld {self.width}x{self.height} agent@({self.agent_x:.1f},{self.agent_y:.1f})>"
