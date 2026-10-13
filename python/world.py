"""World and environment simulation classes"""

import time
from typing import List, Dict, Any, Optional, Union
from dataclasses import dataclass, field
import math

from .types import (
    NeuronId, ActionType, MotorCommand, WorldObjectType,
    SimulationStep, Timestamp, AgentBody
)

@dataclass
class WorldConfig:
    """Configuration for the simulation world"""
    
    # World dimensions
    width: float = 100.0
    height: float = 100.0
    
    # Vision configuration
    vision_width: int = 20
    vision_height: int = 20
    
    # Agent configuration
    agent_start_x: float = 50.0
    agent_start_y: float = 50.0
    
    # Energy system
    max_energy: float = 100.0
    energy_decay_rate: float = 0.01
    resource_energy_gain: float = 10.0
    
    # Simulation parameters
    gravity_enabled: bool = True
    friction_enabled: bool = True
    collision_detection: bool = True
    
    # Random seed for reproducibility
    random_seed: int = 42
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert config to dictionary"""
        return {
            "width": self.width,
            "height": self.height,
            "vision_width": self.vision_width,
            "vision_height": self.vision_height,
            "agent_start_x": self.agent_start_x,
            "agent_start_y": self.agent_start_y,
            "max_energy": self.max_energy,
            "energy_decay_rate": self.energy_decay_rate,
            "resource_energy_gain": self.resource_energy_gain,
            "gravity_enabled": self.gravity_enabled,
            "friction_enabled": self.friction_enabled,
            "collision_detection": self.collision_detection,
            "random_seed": self.random_seed,
        }


@dataclass
class WorldObject:
    """Object in the world - position and type only"""
    
    x: float
    y: float
    radius: float = 0.5
    obj_type: WorldObjectType = WorldObjectType.Empty
    value: float = 0.0
    active: bool = True
    
    def distance_to(self, other_x: float, other_y: float) -> float:
        """Calculate distance to another position"""
        dx = self.x - other_x
        dy = self.y - other_y
        return math.sqrt(dx * dx + dy * dy)
    
    def contains_point(self, px: float, py: float) -> bool:
        """Check if point is inside this object"""
        distance = self.distance_to(px, py)
        return distance <= self.radius


@dataclass
class SensoryPercept:
    """Sensory percept data for agent perception"""
    
    # Vision data (grayscale or RGB)
    vision_data: List[List[float]] = field(default_factory=list)
    vision_width: int = 0
    vision_height: int = 0
    
    # Touch/proprioception data
    touch_data: List[float] = field(default_factory=list)
    proprioception_data: List[float] = field(default_factory=list)
    
    # Internal sensor data
    internal_data: List[float] = field(default_factory=list)
    
    # Audio data (if available)
    audio_data: List[float] = field(default_factory=list)
    
    # Temporal information
    timestamp: Timestamp = 0.0
    
    def get_vision(self, x: int, y: int) -> float:
        """Get vision value at grid position"""
        if 0 <= x < self.vision_width and 0 <= y < self.vision_height:
            return self.vision_data[y][x]
        return 0.0
    
    def get_dimensions(self) -> tuple:
        """Get vision dimensions"""
        return (self.vision_width, self.vision_height)
    
    def get_all_signals(self) -> Dict[str, Any]:
        """Get all sensory signals as dictionary"""
        return {
            "vision": self.vision_data,
            "touch": self.touch_data,
            "proprioception": self.proprioception_data,
            "internal": self.internal_data,
            "audio": self.audio_data,
            "timestamp": self.timestamp,
        }


class WorldSimulationError(Exception):
    """Base exception for world simulation errors"""
    pass


class ConfigurationError(WorldSimulationError):
    """Configuration-related errors"""
    pass


class CollisionError(WorldSimulationError):
    """Collision detection errors"""
    pass


class World:
    """
    Simple 2D world for NLM development and testing.
    
    This class provides a simulated environment where NLM agents can perceive,
    act, and learn. The world provides sensory input (vision, proprioception)
    without semantic labels, allowing the brain to learn meaningful representations.
    
    Key features:
    - 2D continuous space with boundaries
    - Dynamic objects (resources, hazards, markers)
    - Agent physics (position, velocity, energy)
    - Sensory perception (vision, touch, proprioception)
    - Action execution and consequences
    - Collision detection and response
    - Reproducible random seed
    
    The world operates on a discrete time-stepped basis, making it suitable
    for integrating with the event-driven brain simulation.
    
    Example:
        >>> world = SimpleWorld()
        >>> world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
        >>> world.reset()
        >>> world.addObject(WorldObject(50, 50, WorldObjectType.Resource, 10.0))
        
        >>> # Agent perceives world
        >>> percept = world.getSensoryPercept()
        >>> vision = percept.get_vision(10, 10)  # Get vision value
        
        >>> # Agent acts in world
        >>> action = MotorCommand.MoveForward
        >>> result = world.applyMotorCommand(action, 0.1)
    """
    
    def __init__(self, config: Optional[WorldConfig] = None):
        """
        Initialize a new world instance.
        
        Args:
            config: World configuration. If None, uses default config.
            
        Raises:
            ConfigurationError: If configuration is invalid.
            WorldSimulationError: If initialization fails.
        """
        if config is None:
            config = WorldConfig()
            
        self._config = config
        self._validate_config()
        
        # Agent state
        self._agent = AgentBody()
        self._agent.x = config.agent_start_x
        self._agent.y = config.agent_start_y
        self._agent.energy = config.max_energy
        
        # World state
        self._objects: List[WorldObject] = []
        self._sensory = SensoryPercept()
        
        # Simulation state
        self._sim_time = 0.0
        self._step_count = 0
        
        # Random number generator state
        self._rng_seed = config.random_seed
        self._rng_state = config.random_seed
        
        # Initialize vision grid
        self._sensory.vision_width = config.vision_width
        self._sensory.vision_height = config.vision_height
        self._sensory.vision_data = [[0.0 for _ _ in range(config.vision_width)]
                                      for _ _ in range(config.vision_height)]
        
    def _validate_config(self) -> None:
        """Validate world configuration"""
        if self._config.width <= 0:
            raise ConfigurationError("world width must be positive")
        if self._config.height <= 0:
            raise ConfigurationError("world height must be positive")
        if self._config.vision_width <= 0:
            raise ConfigurationError("vision width must be positive")
        if self._config.vision_height <= 0:
            raise ConfigurationError("vision height must be positive")
        if self._config.max_energy <= 0:
            raise ConfigurationError("max energy must be positive")
        if self._config.energy_decay_rate < 0:
            raise ConfigurationError("energy decay rate must be non-negative")
        if self._config.agent_start_x < 0 or self._config.agent_start_x >= self._config.width:
            raise ConfigurationError("agent start x must be within world bounds")
        if self._config.agent_start_y < 0 or self._config.agent_start_y >= self._config.height:
            raise ConfigurationError("agent start y must be within world bounds")
    
    def configure(self, width: float, height: float, 
                  vision_width: int, vision_height: int,
                  agent_start_x: Optional[float] = None,
                  agent_start_y: Optional[float] = None) -> None:
        """
        Configure world parameters.
        
        Args:
            width: World width in units.
            height: World height in units.
            vision_width: Width of agent's visual field in grid cells.
            vision_height: Height of agent's visual field in grid cells.
            agent_start_x: Optional start x position for agent.
            agent_start_y: Optional start y position for agent.
        """
        self._config.width = width
        self._config.height = height
        self._config.vision_width = vision_width
        self._config.vision_height = vision_height
        
        if agent_start_x is not None:
            self._config.agent_start_x = agent_start_x
        if agent_start_y is not None:
            self._config.agent_start_y = agent_start_y
            
        # Update vision data dimensions
        self._sensory.vision_width = vision_width
        self._sensory.vision_height = vision_height
        self._sensory.vision_data = [[0.0 for _ _ in range(vision_width)]
                                      for _ _ in range(vision_height)]
    
    def reset(self) -> None:
        """Reset world to initial state"""
        self._agent.reset()
        self._agent.x = self._config.agent_start_x
        self._agent.y = self._config.agent_start_y
        self._agent.energy = self._config.max_energy
        self._agent.isMoving = False
        self._agent.isTurning = False
        
        self._objects.clear()
        self._sim_time = 0.0
        self._step_count = 0
        
        # Reset vision
        for y in range(self._config.vision_height):
            for x in range(self._config.vision_width):
                self._sensory.vision_data[y][x] = 0.0
        
        self._update_sensory_percept()
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position"""
        if x < 0 or x >= self._config.width:
            raise WorldSimulationError("agent start x out of bounds")
        if y < 0 or y >= self._config.height:
            raise WorldSimulationError("agent start y out of bounds")
            
        self._config.agent_start_x = x
        self._config.agent_start_y = y
    
    def update(self, timestep: float) -> None:
        """
        Update world state for one simulation step.
        
        Args:
            timestep: Time step duration in seconds.
        """
        # Store previous position for collision detection
        prev_x = self._agent.x
        prev_y = self._agent.y
        
        # Apply physics
        self._apply_physics(timestep)
        
        # Check collisions
        if self._config.collision_detection:
            self._check_collisions(prev_x, prev_y)
        
        # Update sensory perception
        self._update_sensory_percept()
        
        # Update simulation time
        self._sim_time += timestep
        self._step_count += 1
    
    def _apply_physics(self, timestep: float) -> None:
        """Apply physics to agent"""
        # Handle agent movement based on action (to be called externally)
        pass
        
        # Energy decay
        self._agent.energy -= self._config.energy_decay_rate * timestep
        if self._agent.energy <= 0:
            self._agent.energy = 0
            self._agent.isMoving = False
        
        # Apply gravity if enabled
        if self._config.gravity_enabled:
            self._agent.velocityY -= 9.8 * timestep  # Simple gravity
            
        # Apply friction if enabled
        if self._config.friction_enabled:
            friction = 0.9
            self._agent.velocityX *= friction
            self._agent.velocityY *= friction
            self._agent.angularVelocity *= friction
    
    def _check_collisions(self, prev_x: float, prev_y: float) -> None:
        """Check for collisions with objects and boundaries"""
        # Check boundaries
        if self._agent.x < 0:
            self._agent.x = 0
            self._agent.velocityX = -self._agent.velocityX * 0.5  # Bounce
        elif self._agent.x >= self._config.width:
            self._agent.x = self._config.width - 0.01
            self._agent.velocityX = -self._agent.velocityX * 0.5
            
        if self._agent.y < 0:
            self._agent.y = 0
            self._agent.velocityY = -self._agent.velocityY * 0.5
        elif self._agent.y >= self._config.height:
            self._agent.y = self._config.height - 0.01
            self._agent.velocityY = -self._agent.velocityY * 0.5
        
        # Check object collisions
        for obj in self._objects:
            if obj.active and obj.contains_point(self._agent.x, self._agent.y):
                # Handle collision (simple bounce)
                dx = self._agent.x - prev_x
                dy = self._agent.y - prev_y
                dist = math.sqrt(dx * dx + dy * dy)
                if dist > 0:
                    nx = dx / dist
                    ny = dy / dist
                    # Push agent away from object
                    push_dist = obj.radius - dist + 0.01
                    self._agent.x += nx * push_dist
                    self._agent.y += ny * push_dist
                    # Reduce velocity (bounce)
                    self._agent.velocityX *= 0.5
                    self._agent.velocityY *= 0.5
    
    def add_object(self, obj: WorldObject) -> None:
        """Add an object to the world"""
        if not obj.active:
            raise WorldSimulationError("cannot add inactive object")
        self._objects.append(obj)
    
    def remove_object(self, x: float, y: float) -> None:
        """Remove object at position"""
        self._objects = [obj for obj in self._objects if not obj.active or 
                        not obj.contains_point(x, y)]
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not wall)"""
        return (0 <= x < self._config.width and 
                0 <= y < self._config.height)
    
    def get_object_at(self, x: float, y: float) -> Optional[WorldObject]:
        """Get object at position (if any)"""
        for obj in self._objects:
            if obj.active and obj.contains_point(x, y):
                return obj
        return None
    
    def apply_motor_command(self, cmd: MotorCommand, current_time: float) -> 'ActionResult':
        """
        Apply motor command to agent and return action result.
        
        Args:
            cmd: Motor command to execute.
            current_time: Current simulation time.
            
        Returns:
            ActionResult with reward, success, and message.
        """
        # Store previous state
        prev_x = self._agent.x
        prev_y = self._agent.y
        prev_energy = self._agent.energy
        
        # Execute motor command
        self._execute_motor_command(cmd, current_time)
        
        # Check for collisions with objects
        collision_objects = []
        for obj in self._objects:
            if obj.active and obj.contains_point(self._agent.x, self._agent.y):
                collision_objects.append(obj)
        
        # Calculate reward and success
        reward = self._calculate_reward(cmd, collision_objects)
        success = self._is_action_successful(cmd, collision_objects)
        
        # Consume energy
        energy_cost = self._get_energy_cost(cmd)
        if energy_cost > self._agent.energy:
            energy_cost = self._agent.energy
        self._agent.energy -= energy_cost
        
        # Handle object interactions
        self._handle_object_interaction(cmd, collision_objects)
        
        # Create action result
        message = self._get_action_message(cmd, success)
        return ActionResult(reward, success, message)
    
    def _execute_motor_command(self, cmd: MotorCommand, current_time: float) -> None:
        """Execute low-level motor command"""
        self._agent.lastActionTime = current_time
        
        # Movement commands
        if cmd == MotorCommand.MoveForward:
            speed = 2.0  # units per second
            angle = self._agent.orientation
            self._agent.velocityX += speed * math.cos(angle)
            self._agent.velocityY += speed * math.sin(angle)
            self._agent.isMoving = True
            
        elif cmd == MotorCommand.MoveBackward:
            speed = 1.0
            angle = self._agent.orientation
            self._agent.velocityX -= speed * math.cos(angle)
            self._agent.velocityY -= speed * math.sin(angle)
            self._agent.isMoving = True
            
        elif cmd == MotorCommand.TurnLeft:
            self._agent.angularVelocity += 0.1  # rad/s
            self._agent.isTurning = True
            self._agent.orientation -= 0.1
            
        elif cmd == MotorCommand.TurnRight:
            self._agent.angularVelocity -= 0.1
            self._agent.isTurning = True
            self._agent.orientation += 0.1
            
        elif cmd == MotorCommand.LookLeft:
            # Adjust vision direction
            pass
            
        elif cmd == MotorCommand.LookRight:
            # Adjust vision direction
            pass
            
        elif cmd == MotorCommand.Interact:
            # Trigger interaction with nearby object
            self._handle_interaction()
            
        elif cmd == MotorCommand.Wait:
            self._agent.isMoving = False
            self._agent.isTurning = False
        
        # Update position based on velocity
        self._agent.x += self._agent.velocityX
        self._agent.y += self._agent.velocityY
        
        # Update orientation
        self._agent.orientation = (self._agent.orientation + 
                                 self._agent.angularVelocity) % (2 * math.pi)
    
    def _calculate_reward(self, cmd: MotorCommand, 
                         collision_objects: List[WorldObject]) -> float:
        """Calculate reward for action"""
        reward = 0.0
        
        # Base reward for movement
        if cmd in [MotorCommand.MoveForward, MotorCommand.TurnLeft, MotorCommand.TurnRight]:
            reward += 0.1
            
        # Reward for resource collection
        for obj in collision_objects:
            if obj.obj_type == WorldObjectType.Resource:
                reward += obj.value
                obj.active = False  # Remove collected resource
            elif obj.obj_type == WorldObjectType.Hazard:
                reward -= obj.value  # Negative reward for hazards
                
        return reward
    
    def _is_action_successful(self, cmd: MotorCommand,
                            collision_objects: List[WorldObject]) -> bool:
        """Determine if action was successful"""
        if cmd == MotorCommand.Interact:
            return len(collision_objects) > 0
        
        if cmd in [MotorCommand.MoveForward, MotorCommand.MoveBackward]:
            return self._agent.isMoving
            
        if cmd in [MotorCommand.TurnLeft, MotorCommand.TurnRight]:
            return self._agent.isTurning
            
        return True  # Wait is always successful
    
    def _get_energy_cost(self, cmd: MotorCommand) -> float:
        """Get energy cost for motor command"""
        energy_costs = {
            MotorCommand.MoveForward: 1.0,
            MotorCommand.MoveBackward: 0.5,
            MotorCommand.TurnLeft: 0.3,
            MotorCommand.TurnRight: 0.3,
            MotorCommand.LookLeft: 0.1,
            MotorCommand.LookRight: 0.1,
            MotorCommand.Interact: 0.5,
            MotorCommand.Wait: 0.0,
        }
        return energy_costs.get(cmd, 0.0)
    
    def _handle_object_interaction(self, cmd: MotorCommand,
                                 collision_objects: List[WorldObject]) -> None:
        """Handle interaction with world objects"""
        if cmd == MotorCommand.Interact and collision_objects:
            # Simple interaction: remove small objects, damage hazards
            for obj in collision_objects:
                if obj.obj_type == WorldObjectType.Resource and obj.radius < 2.0:
                    obj.active = False  # Collect small objects
                elif obj.obj_type == WorldObjectType.Hazard:
                    # Hazard may damage agent
                    damage = obj.value * 0.5
                    if damage > self._agent.health:
                        self._agent.health = 0
                    else:
                        self._agent.health -= damage
    
    def _handle_interaction(self) -> None:
        """Handle interaction action"""
        # Check for nearby objects within interaction range
        interaction_range = 2.0
        nearby_objects = []
        
        for obj in self._objects:
            if obj.active and obj.distance_to(self._agent.x, self._agent.y) <= interaction_range:
                nearby_objects.append(obj)
        
        # Process nearby objects
        for obj in nearby_objects:
            if obj.obj_type == WorldObjectType.Resource:
                # Collect resource
                obj.active = False
                self._agent.energy = min(self._agent.energy + obj.value, 
                                        self._config.max_energy)
            elif obj.obj_type == WorldObjectType.Hazard:
                # Take damage
                damage = obj.value
                if damage > self._agent.health:
                    self._agent.health = 0
                else:
                    self._agent.health -= damage
    
    def _update_sensory_percept(self) -> None:
        """Update agent's sensory perception based on current state"""
        # Clear vision data
        for y in range(self._config.vision_height):
            for x in range(self._config.vision_width):
                self._sensory.vision_data[y][x] = 0.0
        
        # Convert agent position to vision coordinates
        agent_grid_x = int(self._agent.x / self._config.width * self._config.vision_width)
        agent_grid_y = int(self._agent.y / self._config.height * self._config.vision_height)
        
        # Sample vision around agent
        vision_range = 1
        for dy in range(-vision_range, vision_range + 1):
            for dx in range(-vision_range, vision_range + 1):
                grid_x = agent_grid_x + dx
                grid_y = agent_grid_y + dy
                
                if 0 <= grid_x < self._config.vision_width and 0 <= grid_y < self._config.vision_height:
                    # Check for objects at this vision cell
                    cell_x = int(grid_x / self._config.vision_width * self._config.width)
                    cell_y = int(grid_y / self._config.vision_height * self._config.height)
                    
                    # Sample vision based on world state
                    vision_value = self._sample_vision_at(cell_x, cell_y)
                    self._sensory.vision_data[grid_y][grid_x] = vision_value
    
    def _sample_vision_at(self, x: float, y: float) -> float:
        """Sample vision value at world coordinates"""
        # Sample from objects
        for obj in self._objects:
            if obj.active and obj.contains_point(x, y):
                # Return object type as vision value
                return {
                    WorldObjectType.Empty: 0.0,
                    WorldObjectType.Resource: 0.8,
                    WorldObjectType.Hazard: 0.3,
                    WorldObjectType.Wall: 0.9,
                    WorldObjectType.Marker: 0.5,
                }.get(obj.obj_type, 0.0)
        
        # Return empty space
        return 0.0
    
    # Getters and property access
    def get_sensory_percept(self) -> SensoryPercept:
        """Get current sensory percept"""
        return self._sensory
        
    def get_agent_body(self) -> AgentBody:
        """Get agent body state"""
        return self._agent
        
    def get_width(self) -> float:
        """Get world width"""
        return self._config.width
        
    def get_height(self) -> float:
        """Get world height"""
        return self._config.height
        
    def get_max_energy(self) -> float:
        """Get maximum agent energy"""
        return self._config.max_energy
        
    def set_max_energy(self, energy: float) -> None:
        """Set maximum agent energy"""
        self._config.max_energy = energy
        
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate"""
        return self._config.energy_decay_rate
        
    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate"""
        self._config.energy_decay_rate = rate
        
    def get_simulation_time(self) -> float:
        """Get current simulation time"""
        return self._sim_time
        
    def get_step_count(self) -> int:
        """Get number of simulation steps"""
        return self._step_count
        
    def set_random_seed(self, seed: int) -> None:
        """Set random seed for reproducible simulations"""
        self._config.random_seed = seed
        self._rng_seed = seed
        self._rng_state = seed
        
    def get_random_seed(self) -> int:
        """Get random seed"""
        return self._config.random_seed
        
    def _next_random(self) -> float:
        """Generate next random number in [0, 1)"""
        # Simple linear congruential generator
        self._rng_state = (1664525 * self._rng_state + 1013904223) % 2**32
        return self._rng_state / 2**32
        
    def log_status(self) -> None:
        """Log world status and statistics"""
        print(f"=== Simple World Status ===")
        print(f"Dimensions: {self._config.width}x{self._config.height}")
        print(f"Simulation time: {self._sim_time:.2f}s")
        print(f"Steps: {self._step_count}")
        print(f"Agent position: ({self._agent.x:.1f}, {self._agent.y:.1f})")
        print(f"Agent orientation: {self._agent.orientation:.2f} rad")
        print(f"Agent energy: {self._agent.energy:.1f}/{self._config.max_energy}")
        print(f"Active objects: {sum(1 for obj in self._objects if obj.active)}")
        print(f"Objects: {len(self._objects)}")
        
        # Log some vision data
        print(f"Vision range: {self._config.vision_width}x{self._config.vision_height}")
        
        # Log object distribution
        obj_counts = {}
        for obj in self._objects:
            obj_type = obj.obj_type
            obj_counts[obj_type] = obj_counts.get(obj_type, 0) + 1
            
        print("Object distribution:")
        for obj_type, count in obj_counts.items():
            print(f"  {obj_type}: {count}")


class ActionResult:
    """Result of an action applied to the world"""
    
    def __init__(self, reward: float, success: bool, message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message
        
    def __str__(self) -> str:
        return f"ActionResult(reward={self.reward:.2f}, success={self.success}, message='{self.message}')"
        
    def __repr__(self) -> str:
        return self.__str__()


# Factory functions
def create_default_world_config() -> WorldConfig:
    """Create a default world configuration"""
    return WorldConfig()


def create_simple_world(config: Optional[WorldConfig] = None) -> World:
    """Create a new simple world instance"""
    return World(config)


def create_agent_world(brain_agent, config: Optional[WorldConfig] = None) -> 'AgentWorld':
    """
    Create a world with an integrated agent.
    
    Args:
        brain_agent: AgentBrain instance for the world.
        config: World configuration.
        
    Returns:
        AgentWorld instance.
    """
    return AgentWorld(brain_agent, config)


# Export commonly used classes and functions
__all__ = [
    'WorldConfig',
    'WorldObject',
    'SensoryPercept',
    'World',
    'ActionResult',
    'create_default_world_config',
    'create_simple_world',
    'create_agent_world',
    'WorldSimulationError',
    'ConfigurationError',
    'CollisionError',
]
