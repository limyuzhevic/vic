# World module for NLM
from typing import Optional, List
import time
import math
from enum import Enum

# Type aliases for Python API
NeuronId = int
RegionId = int
MotorCommand = int
WorldObjectType = int
ActionType = int
Timestamp = float

# Enumerations
class ActionType(Enum):
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

class MotorCommand(Enum):
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType(Enum):
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

class ActionResult:
    """Action result from world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message
        self.timestamp = time.time()

class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty, 
                 value: float = 0.0, radius: float = 0.5):
        self.x = x
        self.y = y
        self.radius = radius
        self.type = obj_type
        self.value = value  # Reward value (for resources) or damage (for hazards)
        self.active = True
    
    def __repr__(self):
        return f"WorldObject(x={self.x}, y={self.y}, type={self.type.name}, value={self.value})"

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
        self.health = 100.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0

class SensoryPercept:
    """Sensory percept data."""
    
    def __init__(self):
        self._vision = None
        self._touch = None
        self._internal = None
        self._proprioception = None
        self._audio = None
        self._timestamp = time.time()
    
    def setVision(self, vision_data) -> None:
        """Set vision data."""
        self._vision = vision_data
    
    def getVision(self):
        """Get vision data."""
        return self._vision
    
    def getVisionWidth(self) -> int:
        """Get vision width."""
        if hasattr(self._vision, 'getWidth'):
            return self._vision.getWidth()
        return 0
    
    def getVisionHeight(self) -> int:
        """Get vision height."""
        if hasattr(self._vision, 'getHeight'):
            return self._vision.getHeight()
        return 0
    
    def setTouch(self, touch_data) -> None:
        """Set touch data."""
        self._touch = touch_data
    
    def getTouch(self):
        """Get touch data."""
        return self._touch
    
    def setInternal(self, internal_data) -> None:
        """Set internal signal data."""
        self._internal = internal_data
    
    def getInternal(self):
        """Get internal signal data."""
        return self._internal
    
    def setProprioception(self, proprioception_data) -> None:
        """Set proprioception data."""
        self._proprioception = proprioception_data
    
    def getProprioception(self):
        """Get proprioception data."""
        return self._proprioception
    
    def setAudio(self, audio_data) -> None:
        """Set audio data."""
        self._audio = audio_data
    
    def getAudio(self):
        """Get audio data."""
        return self._audio
    
    def getAllSignals(self) -> dict:
        """Get all sensory signals."""
        signals = {}
        if self._vision is not None:
            signals['vision'] = self._vision
        if self._touch is not None:
            signals['touch'] = self._touch
        if self._internal is not None:
            signals['internal'] = self._internal
        if self._proprioception is not None:
            signals['proprioception'] = self._proprioception
        if self._audio is not None:
            signals['audio'] = self._audio
        return signals
    
    def getTimestamp(self) -> float:
        """Get timestamp."""
        return self._timestamp
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set timestamp."""
        self._timestamp = timestamp

class SimpleWorld:
    """
    Simple 2D world for NLM simulation.
    
    Provides a 2D environment for agent interaction with objects,
    providing sensory input through vision, touch, and internal signals.
    """
    
    def __init__(self):
        self._width = 10.0
        self._height = 10.0
        self._vision_width = 8
        self._vision_height = 8
        
        # Agent
        self._agent = AgentBody()
        
        # Objects in world
        self._objects: List[WorldObject] = []
        
        # Sensory state
        self._sensory = SensoryPercept()
        
        # Energy system
        self._max_energy = 100.0
        self._energy_decay_rate = 0.1
        self._resource_energy_gain = 10.0
        
        # Simulation time
        self._sim_time = 0.0
        
        # Random seed
        self._rng_seed = 42
        self._rng_state = 123456789
        
        # Initialize
        self._reset_objects()
        self._generate_vision()
    
    def configure(self, width: float, height: float, visionWidth: int, visionHeight: int) -> None:
        """
        Configure world dimensions.
        
        Args:
            width: World width
            height: World height
            visionWidth: Vision sensor width
            visionHeight: Vision sensor height
        """
        self._width = width
        self._height = height
        self._vision_width = visionWidth
        self._vision_height = visionHeight
        
        # Re-generate vision with new dimensions
        self._generate_vision()
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self._sim_time = 0.0
        self._agent.x = self._width / 2
        self._agent.y = self._height / 2
        self._agent.orientation = 0.0
        self._agent.velocityX = 0.0
        self._agent.velocityY = 0.0
        self._agent.energy = self._max_energy
        self._agent.health = self._max_energy
        self._agent.age = 0.0
        self._agent.isMoving = False
        self._agent.isTurning = False
        self._agent.lastActionTime = 0.0
        
        self._reset_objects()
        self._generate_vision()
    
    def setAgentStart(self, x: float, y: float) -> None:
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._agent.x = x
        self._agent.y = y
    
    def update(self, timestep: float) -> None:
        """
        Update world state.
        
        Args:
            timestep: Time step duration
        """
        self._sim_time += timestep
        self._agent.age += timestep
        
        # Update agent energy (decay over time)
        if self._energy_decay_rate > 0:
            energy_loss = self._energy_decay_rate * timestep
            self._agent.energy = max(0.0, self._agent.energy - energy_loss)
        
        # Check for agent death
        if self._agent.energy <= 0 or self._agent.health <= 0:
            self._agent.isMoving = False
        
        # Generate new sensory input
        self._generate_vision()
        self._check_collisions()
    
    def applyMotorCommand(self, cmd: MotorCommand, currentTime: float) -> ActionResult:
        """
        Apply motor command to agent.
        
        Args:
            cmd: Motor command to apply
            currentTime: Current simulation time
            
        Returns:
            ActionResult: Result of the action
        """
        result = ActionResult()
        
        # Store previous position for collision detection
        prev_x = self._agent.x
        prev_y = self._agent.y
        
        # Apply movement based on command
        if cmd == MotorCommand.MoveForward:
            angle_rad = math.radians(self._agent.orientation)
            self._agent.velocityX += math.cos(angle_rad) * 0.5
            self._agent.velocityY += math.sin(angle_rad) * 0.5
        elif cmd == MotorCommand.MoveBackward:
            angle_rad = math.radians(self._agent.orientation)
            self._agent.velocityX -= math.cos(angle_rad) * 0.5
            self._agent.velocityY -= math.sin(angle_rad) * 0.5
        elif cmd == MotorCommand.TurnLeft:
            self._agent.orientation -= 10.0
            self._agent.isTurning = True
        elif cmd == MotorCommand.TurnRight:
            self._agent.orientation += 10.0
            self._agent.isTurning = True
        elif cmd == MotorCommand.Interact:
            result = self._interact()
        
        # Update agent position
        self._agent.x += self._agent.velocityX * timestep
        self._agent.y += self._agent.velocityY * timestep
        
        # Wrap around world boundaries
        self._agent.x = max(0, min(self._width, self._agent.x))
        self._agent.y = max(0, min(self._height, self._agent.y))
        
        # Update movement flag
        self._agent.isMoving = (self._agent.velocityX != 0 or self._agent.velocityY != 0)
        
        # Check for collisions
        if not self._check_collisions():
            # Collision occurred - revert position
            self._agent.x = prev_x
            self._agent.y = prev_y
            result.success = False
            result.message = "Collision with wall or obstacle"
        else:
            result.success = True
            result.message = f"Executed {cmd.name}"
        
        # Apply energy changes based on action
        if cmd == MotorCommand.MoveForward or cmd == MotorCommand.MoveBackward:
            energy_loss = 1.0  # Movement costs energy
            self._agent.energy = max(0.0, self._agent.energy - energy_loss)
        
        self._agent.lastActionTime = currentTime
        self._generate_vision()
        
        return result
    
    def getSensoryPercept(self) -> SensoryPercept:
        """
        Get current sensory percept for the agent.
        
        Returns:
            SensoryPercept: Current sensory percept
        """
        return self._sensory
    
    def getAgentBody(self) -> AgentBody:
        """
        Get agent body state.
        
        Returns:
            AgentBody: Current agent body state
        """
        return self._agent
    
    def addObject(self, obj: WorldObject) -> None:
        """
        Add object to world.
        
        Args:
            obj: World object to add
        """
        self._objects.append(obj)
    
    def removeObject(self, x: float, y: float) -> None:
        """
        Remove object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._objects = [obj for obj in self._objects if not (abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """
        Check if position is valid (not wall).
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            bool: True if position is valid
        """
        # Check for walls
        if x < 0 or x > self._width or y < 0 or y > self._height:
            return False
        
        # Check for other objects
        for obj in self._objects:
            if obj.type == WorldObjectType.Wall:
                dist = math.sqrt((obj.x - x) ** 2 + (obj.y - y) ** 2)
                if dist < obj.radius + 0.1:  # Small buffer
                    return False
        
        return True
    
    def getWidth(self) -> float:
        """Get world width."""
        return self._width
    
    def getHeight(self) -> float:
        """Get world height."""
        return self._height
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy."""
        return self._max_energy
    
    def setMaxEnergy(self, energy: float) -> None:
        """
        Set maximum energy.
        
        Args:
            energy: Maximum energy
        """
        self._max_energy = energy
        self._agent.energy = min(self._agent.energy, energy)
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate."""
        return self._energy_decay_rate
    
    def setEnergyDecayRate(self, rate: float) -> None:
        """
        Set energy decay rate.
        
        Args:
            rate: Energy decay rate
        """
        self._energy_decay_rate = rate
    
    def getSimulationTime(self) -> float:
        """Get simulation time."""
        return self._sim_time
    
    def setRandomSeed(self, seed: int) -> None:
        """
        Set random seed.
        
        Args:
            seed: Random seed
        """
        self._rng_seed = seed
        self._rng_state = hash(str(seed)) & 0xFFFFFFFF
    
    def getRandomSeed(self) -> int:
        """Get random seed."""
        return self._rng_seed
    
    # Internal helper methods
    
    def _reset_objects(self) -> None:
        """Reset objects to initial state."""
        self._objects.clear()
        
        # Add some default objects
        # Add walls
        for x in [0, self._width]:
            for y in [0, self._height]:
                obj = WorldObject(x, y, WorldObjectType.Wall, 0.0, 0.2)
                self._objects.append(obj)
        
        # Add some resources
        import random
        for _ in range(5):
            x = random.uniform(1, self._width - 1)
            y = random.uniform(1, self._height - 1)
            obj = WorldObject(x, y, WorldObjectType.Resource, 10.0, 0.3)
            self._objects.append(obj)
        
        # Add some hazards
        for _ in range(3):
            x = random.uniform(1, self._width - 1)
            y = random.uniform(1, self._height - 1)
            obj = WorldObject(x, y, WorldObjectType.Hazard, -5.0, 0.3)
            self._objects.append(obj)
    
    def _generate_vision(self) -> None:
        """Generate vision from current view."""
        # Create a simple vision representation
        vision_data = {
            'width': self._vision_width,
            'height': self._vision_height,
            'data': [],
            'agent_x': self._agent.x,
            'agent_y': self._agent.y,
            'orientation': self._agent.orientation
        }
        
        # Sample vision in front of agent
        angle_rad = math.radians(self._agent.orientation)
        vision_distance = 5.0
        
        for i in range(self._vision_height):
            for j in range(self._vision_width):
                # Convert pixel coordinates to world coordinates
                rel_x = (j / (self._vision_width - 1)) * vision_distance
                rel_y = (i / (self._vision_height - 1)) * vision_distance
                
                # Rotate based on agent orientation
                world_x = self._agent.x + rel_x * math.cos(angle_rad) - rel_y * math.sin(angle_rad)
                world_y = self._agent.y + rel_x * math.sin(angle_rad) + rel_y * math.cos(angle_rad)
                
                # Find object at this position
                intensity = 0.0
                for obj in self._objects:
                    dist = math.sqrt((world_x - obj.x) ** 2 + (world_y - obj.y) ** 2)
                    if dist < obj.radius + 0.1:
                        intensity = obj.value / 100.0  # Normalize
                        break
                
                vision_data['data'].append(intensity)
        
        # Create vision object
        vision = type('Vision', (), {
            'getWidth': lambda: self._vision_width,
            'getHeight': lambda: self._vision_height,
            'getData': lambda: vision_data['data'],
            'getIntensity': lambda idx: vision_data['data'][idx]
        })()
        
        self._sensory.setVision(vision)
    
    def _check_collisions(self) -> bool:
        """
        Check for collisions.
        
        Returns:
            bool: True if no collision
        """
        # Check world boundaries
        if self._agent.x < 0 or self._agent.x > self._width:
            return False
        
        if self._agent.y < 0 or self._agent.y > self._height:
            return False
        
        # Check collisions with objects
        for obj in self._objects:
            dist = math.sqrt((self._agent.x - obj.x) ** 2 + (self._agent.y - obj.y) ** 2)
            if dist < obj.radius + 0.1:
                # Collision with wall
                if obj.type == WorldObjectType.Wall:
                    return False
                # Collect resource or get damaged by hazard
                if obj.type == WorldObjectType.Resource and obj.value > 0:
                    self._agent.energy = min(self._agent.energy + obj.value, self._max_energy)
                    self._objects.remove(obj)
                elif obj.type == WorldObjectType.Hazard:
                    self._agent.health += obj.value  # Damage
                    self._agent.energy += obj.value * 0.5
        
        return True
    
    def _interact(self) -> ActionResult:
        """Interact with nearby objects."""
        result = ActionResult()
        
        # Find nearest object
        nearest_obj = None
        nearest_dist = float('inf')
        
        for obj in self._objects:
            dist = math.sqrt((self._agent.x - obj.x) ** 2 + (self._agent.y - obj.y) ** 2)
            if dist < obj.radius + 0.5 and dist < nearest_dist:
                nearest_obj = obj
                nearest_dist = dist
        
        if nearest_obj:
            if nearest_obj.type == WorldObjectType.Resource:
                self._agent.energy = min(self._agent.energy + nearest_obj.value, self._max_energy)
                self._objects.remove(nearest_obj)
                result.success = True
                result.reward = nearest_obj.value
                result.message = f"Collected {nearest_obj.value} energy from resource"
            elif nearest_obj.type == WorldObjectType.Hazard:
                self._agent.health += nearest_obj.value
                self._agent.energy += nearest_obj.value * 0.5
                result.success = True
                result.reward = nearest_obj.value * 0.5
                result.message = f"Affected by hazard (health: {self._agent.health}, energy: {self._agent.energy})"
            else:
                result.success = True
                result.message = f"Interacted with {nearest_obj.type.name}"
        else:
            result.success = False
            result.message = "No object to interact with"
        
        return result

# Factory function for backward compatibility
def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world.
    
    Returns:
        SimpleWorld: New simple world instance
    """
    return SimpleWorld()
