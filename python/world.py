"""
World-related classes and functions for NLM Python bindings.
"""

from typing import List, Optional, Dict, Any, Tuple
import math
import structlog

# Get the logger
logger = structlog.get_logger()

from .brain import Action, ActionTypes, SensorInput
from .agent import AgentBrain, ActionResult, AgentBody

# Forward references for type annotations
class Config:
    pass

class Vision:
    pass

class Audio:
    pass

class InternalSignals:
    pass

class WorldObject:
    pass

# Constants
DEFAULT_WORLD_WIDTH = 10
DEFAULT_WORLD_HEIGHT = 10
DEFAULT_VISION_WIDTH = 8
DEFAULT_VISION_HEIGHT = 8
DEFAULT_VISION_CHANNELS = 3
MAX_ENERGY = 100.0
ENERGY_DECAY_RATE = 0.01

# World object types
class WorldObjectTypes:
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
    AGENT = "agent"
    FOOD = "food"
    WATER = "water"
    SUNLIGHT = "sunlight"
    TOXIN = "toxin"

# Exception classes
class WorldError(Exception):
    """Base exception for world errors."""
    pass

class InvalidPositionError(WorldError):
    """Raised when position is invalid."""
    pass

class WorldConfigurationError(WorldError):
    """Raised when world configuration is invalid."""
    pass

# Vector operations
class Vector2D:
    """2D vector for physics calculations."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0):
        self.x = x
        self.y = y
    
    def __add__(self, other):
        return Vector2D(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other):
        return Vector2D(self.x - other.x, self.y - other.y)
    
    def __mul__(self, scalar: float):
        return Vector2D(self.x * scalar, self.y * scalar)
    
    def __truediv__(self, scalar: float):
        return Vector2D(self.x / scalar, self.y / scalar)
    
    def magnitude(self) -> float:
        """Get vector magnitude."""
        return math.sqrt(self.x**2 + self.y**2)
    
    def normalize(self):
        """Normalize vector to unit length."""
        mag = self.magnitude()
        if mag > 0:
            self.x /= mag
            self.y /= mag
        return self
    
    def distance_to(self, other) -> float:
        """Get distance to another vector.
        
        Args:
            other: Other vector.
            
        Returns:
            float: Distance.
        """
        return (self - other).magnitude()
    
    def angle_to(self, other) -> float:
        """Get angle to another vector (radians).
        
        Args:
            other: Other vector.
            
        Returns:
            float: Angle in radians.
        """
        return math.atan2(other.y - self.y, other.x - self.x)
    
    def rotate(self, angle: float):
        """Rotate vector by angle (radians).
        
        Args:
            angle: Rotation angle in radians.
        """
        cos_angle = math.cos(angle)
        sin_angle = math.sin(angle)
        old_x = self.x
        self.x = old_x * cos_angle - self.y * sin_angle
        self.y = old_x * sin_angle + self.y * cos_angle
    
    def __repr__(self) -> str:
        return f"Vector2D({self.x:.2f}, {self.y:.2f})"


class WorldObject:
    """World object representing entities in the simulation world."""
    
    def __init__(self, x: float, y: float, object_type: str = WorldObjectTypes.EMPTY, 
                 value: float = 0.0, radius: float = 0.5, active: bool = True,
                 mass: float = 1.0, velocity_x: float = 0.0, velocity_y: float = 0.0):
        self._x = x
        self._y = y
        self._type = object_type
        self._value = value
        self._radius = radius
        self._active = active
        self._mass = mass
        self._velocity_x = velocity_x
        self._velocity_y = velocity_y
        
        # Validation
        if not isinstance(object_type, str):
            raise WorldError(f"Object type must be string, got {type(object_type)}")
        
        if radius < 0:
            raise InvalidPositionError("Radius cannot be negative")
        
        logger.debug("WorldObject created", 
                    x=x, y=y, type=object_type, radius=radius)
    
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
    def type(self) -> str:
        """Get object type."""
        return self._type
    
    @type.setter
    def type(self, value: str):
        """Set object type."""
        valid_types = [
            WorldObjectTypes.EMPTY,
            WorldObjectTypes.RESOURCE,
            WorldObjectTypes.HAZARD,
            WorldObjectTypes.WALL,
            WorldObjectTypes.MARKER,
            WorldObjectTypes.AGENT,
            WorldObjectTypes.FOOD,
            WorldObjectTypes.WATER,
            WorldObjectTypes.SUNLIGHT,
            WorldObjectTypes.TOXIN
        ]
        if value not in valid_types:
            raise ValueError(f"Invalid world object type: {value}")
        self._type = value
    
    @property
    def value(self) -> float:
        """Get object value."""
        return self._value
    
    @value.setter
    def value(self, value: float):
        """Set object value."""
        self._value = value
    
    @property
    def radius(self) -> float:
        """Get object radius."""
        return self._radius
    
    @radius.setter
    def radius(self, value: float):
        """Set object radius."""
        if value < 0:
            raise InvalidPositionError("Radius cannot be negative")
        self._radius = value
    
    @property
    def active(self) -> bool:
        """Get active status."""
        return self._active
    
    @active.setter
    def active(self, value: bool):
        """Set active status."""
        self._active = value
    
    @property
    def mass(self) -> float:
        """Get object mass."""
        return self._mass
    
    @mass.setter
    def mass(self, value: float):
        """Set object mass."""
        if value <= 0:
            raise ValueError("Mass must be positive")
        self._mass = value
    
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
    
    def get_position(self) -> Tuple[float, float]:
        """Get object position.
        
        Returns:
            Tuple[float, float]: (x, y) position.
        """
        return (self._x, self._y)
    
    def set_position(self, x: float, y: float):
        """Set object position.
        
        Args:
            x: X position.
            y: Y position.
        """
        self._x = x
        self._y = y
    
    def get_velocity(self) -> Tuple[float, float]:
        """Get object velocity.
        
        Returns:
            Tuple[float, float]: (vx, vy) velocity.
        """
        return (self._velocity_x, self._velocity_y)
    
    def set_velocity(self, vx: float, vy: float):
        """Set object velocity.
        
        Args:
            vx: X velocity.
            vy: Y velocity.
        """
        self._velocity_x = vx
        self._velocity_y = vy
    
    def get_distance_to(self, other) -> float:
        """Get distance to another object.
        
        Args:
            other: Other WorldObject.
            
        Returns:
            float: Distance.
        """
        dx = other._x - self._x
        dy = other._y - self._y
        return math.sqrt(dx**2 + dy**2)
    
    def get_angle_to(self, other) -> float:
        """Get angle to another object (radians).
        
        Args:
            other: Other WorldObject.
            
        Returns:
            float: Angle in radians.
        """
        return math.atan2(other._y - self._y, other._x - self._x)
    
    def is_colliding(self, other) -> bool:
        """Check if object is colliding with another object.
        
        Args:
            other: Other WorldObject.
            
        Returns:
            bool: True if colliding.
        """
        distance = self.get_distance_to(other)
        return distance <= (self._radius + other._radius)
    
    def move_to(self, target_x: float, target_y: float, speed: float = 1.0, timestep: float = 0.1):
        """Move object toward target position.
        
        Args:
            target_x: Target x position.
            target_y: Target y position.
            speed: Movement speed.
            timestep: Time step duration.
        """
        dx = target_x - self._x
        dy = target_y - self._y
        distance = math.sqrt(dx**2 + dy**2)
        
        if distance > 0:
            # Calculate direction
            direction_x = dx / distance
            direction_y = dy / distance
            
            # Calculate movement
            move_x = direction_x * speed * timestep
            move_y = direction_y * speed * timestep
            
            # Update position
            self._x += move_x
            self._y += move_y
            
            logger.debug("Object moved", 
                        object_type=self._type,
                        from_pos=(self._x - move_x, self._y - move_y),
                        to_pos=(self._x, self._y),
                        distance=distance,
                        speed=speed)
    
    def rotate_toward(self, target_x: float, target_y: float, angular_speed: float = math.pi, timestep: float = 0.1):
        """Rotate object toward target position.
        
        Args:
            target_x: Target x position.
            target_y: Target y position.
            angular_speed: Angular speed (rad/s).
            timestep: Time step duration.
        """
        angle = self.get_angle_to(type('Other', (), {'_x': target_x, '_y': target_y})())
        self.rotate_toward_angle(angle, angular_speed, timestep)
    
    def rotate_toward_angle(self, target_angle: float, angular_speed: float = math.pi, timestep: float = 0.1):
        """Rotate object toward target angle.
        
        Args:
            target_angle: Target angle (radians).
            angular_speed: Angular speed (rad/s).
            timestep: Time step duration.
        """
        # This would require orientation property, simplified for now
        pass
    
    def get_vector_to(self, other) -> Vector2D:
        """Get vector to another object.
        
        Args:
            other: Other WorldObject.
            
        Returns:
            Vector2D: Vector to other object.
        """
        return Vector2D(other._x - self._x, other._y - self._y)
    
    def is_in_range(self, x: float, y: float, range_distance: float) -> bool:
        """Check if object is within range of position.
        
        Args:
            x: X position.
            y: Y position.
            range_distance: Range distance.
            
        Returns:
            bool: True if within range.
        """
        distance = self.get_distance_to(type('Other', (), {'_x': x, '_y': y})())
        return distance <= range_distance
    
    def is_agent_interactable(self) -> bool:
        """Check if agent can interact with this object.
        
        Returns:
            bool: True if interactable.
        """
        return self._type in [WorldObjectTypes.RESOURCE, WorldObjectTypes.FOOD, 
                            WorldObjectTypes.WATER, WorldObjectTypes.SUNLIGHT] and self._active
    
    def __repr__(self) -> str:
        return f"<WorldObject x={self._x:.2f} y={self._y:.2f} type={self._type} radius={self._radius:.2f}>"


class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        self._width = DEFAULT_WORLD_WIDTH
        self._height = DEFAULT_WORLD_HEIGHT
        self._vision_width = DEFAULT_VISION_WIDTH
        self._vision_height = DEFAULT_VISION_HEIGHT
        self._vision_channels = DEFAULT_VISION_CHANNELS
        
        # Agent state
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        self._agent_velocity_x = 0.0
        self._agent_velocity_y = 0.0
        self._agent_angular_velocity = 0.0
        self._agent_energy = MAX_ENERGY
        self._agent_health = 100.0
        self._agent_age = 0
        self._agent_is_moving = False
        self._agent_is_turning = False
        self._agent_last_action_time = 0.0
        
        # World state
        self._simulation_time = 0.0
        self._objects = {}
        self._world_objects = []  # List of WorldObject instances
        
        # Add default agent object
        agent_object = WorldObject(self._agent_x, self._agent_y, WorldObjectTypes.AGENT)
        agent_object.mass = 1.0
        self._objects['agent'] = agent_object
        self._world_objects.append(agent_object)
        
        logger.debug("SimpleWorld created", 
                    width=self._width, height=self._height)
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions.
        
        Args:
            width: World width.
            height: World height.
            vision_width: Vision width.
            vision_height: Vision height.
            
        Raises:
            WorldConfigurationError: If configuration is invalid.
        """
        if width <= 0 or height <= 0:
            raise WorldConfigurationError("World dimensions must be positive")
        
        if vision_width <= 0 or vision_height <= 0:
            raise WorldConfigurationError("Vision dimensions must be positive")
        
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        
        logger.debug("World configured", 
                    width=width, height=height,
                    vision_width=vision_width, vision_height=vision_height)
    
    def reset(self):
        """Reset world state."""
        self._simulation_time = 0.0
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        self._agent_velocity_x = 0.0
        self._agent_velocity_y = 0.0
        self._agent_angular_velocity = 0.0
        self._agent_energy = MAX_ENERGY
        self._agent_health = 100.0
        self._agent_age = 0
        self._agent_is_moving = False
        self._agent_is_turning = False
        self._agent_last_action_time = 0.0
        
        # Update agent object
        if 'agent' in self._objects:
            self._objects['agent'].set_position(self._agent_x, self._agent_y)
        
        logger.debug("World reset")
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position.
        
        Args:
            x: Starting x position.
            y: Starting y position.
            
        Raises:
            InvalidPositionError: If position is invalid.
        """
        if not self._is_valid_position(x, y):
            raise InvalidPositionError(f"Position ({x}, {y}) is invalid")
        
        self._agent_x = x
        self._agent_y = y
        
        if 'agent' in self._objects:
            self._objects['agent'].set_position(x, y)
        
        logger.debug("Agent start position set", x=x, y=y)
    
    def update(self, timestep: float):
        """Update world state.
        
        Args:
            timestep: Time step duration.
        """
        if timestep <= 0:
            raise ValueError("Timestep must be positive")
        
        self._simulation_time += timestep
        self._agent_age += 1
        
        # Update agent energy (decay over time)
        self._agent_energy -= self._agent_energy * ENERGY_DECAY_RATE * timestep
        if self._agent_energy < 0:
            self._agent_energy = 0
        
        # Check for world events
        self._check_world_events(timestep)
        
        logger.debug("World updated", 
                    timestep=timestep,
                    time=self._simulation_time,
                    agent_energy=self._agent_energy,
                    age=self._agent_age)
    
    def _check_world_events(self, timestep: float):
        """Check for world events and trigger them.
        
        Args:
            timestep: Time step duration.
        """
        # Check for agent position updates
        if 'agent' in self._objects:
            agent_obj = self._objects['agent']
            self._agent_x = agent_obj.x
            self._agent_y = agent_obj.y
        
        # Check for agent resource collection
        self._check_resource_collection()
        
        # Check for agent hazards
        self._check_agent_hazards()
    
    def _check_resource_collection(self):
        """Check if agent has collected resources."""
        collected_resources = []
        
        for obj_name, obj in self._objects.items():
            if obj.is_agent_interactable() and obj.get_distance_to(self._get_agent_object()) <= obj.radius + 0.5:
                collected_resources.append(obj)
        
        for obj in collected_resources:
            self._collect_resource(obj)
            self._objects.pop(obj_name, None)
            self._world_objects.remove(obj)
    
    def _collect_resource(self, resource_obj: WorldObject):
        """Collect resource and apply effects.
        
        Args:
            resource_obj: Resource object to collect.
        """
        if resource_obj.type == WorldObjectTypes.FOOD:
            self._agent_energy = min(MAX_ENERGY, self._agent_energy + 30.0)
        elif resource_obj.type == WorldObjectTypes.WATER:
            self._agent_energy = min(MAX_ENERGY, self._agent_energy + 20.0)
        elif resource_obj.type == WorldObjectTypes.SUNLIGHT:
            self._agent_energy = min(MAX_ENERGY, self._agent_energy + 15.0)
        
        logger.debug("Resource collected", 
                    resource_type=resource_obj.type,
                    energy_increase=30.0 if resource_obj.type == WorldObjectTypes.FOOD else 
                                   20.0 if resource_obj.type == WorldObjectTypes.WATER else 15.0)
    
    def _check_agent_hazards(self):
        """Check if agent is in hazardous areas."""
        hazard_objects = []
        
        for obj_name, obj in self._objects.items():
            if obj.type in [WorldObjectTypes.HAZARD, WorldObjectTypes.TOXIN] and \
               obj.get_distance_to(self._get_agent_object()) <= obj.radius + 0.5:
                hazard_objects.append(obj)
        
        for obj in hazard_objects:
            self._apply_hazard_damage(obj)
    
    def _apply_hazard_damage(self, hazard_obj: WorldObject):
        """Apply damage from hazard.
        
        Args:
            hazard_obj: Hazard object.
        """
        damage = 10.0 if hazard_obj.type == WorldObjectTypes.HAZARD else 20.0
        self._agent_health -= damage
        
        if self._agent_health <= 0:
            self._agent_health = 0
            logger.debug("Agent died from hazard damage")
        
        logger.debug("Hazard damage applied", 
                    hazard_type=hazard_obj.type,
                    damage=damage,
                    remaining_health=self._agent_health)
    
    def apply_motor_command(self, action: Action, current_time: float) -> ActionResult:
        """Apply motor command in world.
        
        Args:
            action: Motor command to apply.
            current_time: Current simulation time.
            
        Returns:
            ActionResult: Result of action application.
            
        Raises:
            ValueError: If action is invalid.
        """
        if action is None:
            raise ValueError("Action cannot be None")
        
        action_type = action.type
        action_params = action.parameters
        
        # Calculate movement based on action
        move_x = 0.0
        move_y = 0.0
        angular_velocity = 0.0
        energy_cost = 0.0
        
        # Action-specific movement calculations
        if action_type == ActionTypes.MOVE_FORWARD:
            angle = self._agent_orientation
            move_x = math.cos(angle) * 2.0
            move_y = math.sin(angle) * 2.0
            energy_cost = 5.0
        elif action_type == ActionTypes.MOVE_BACKWARD:
            angle = self._agent_orientation
            move_x = -math.cos(angle) * 2.0
            move_y = -math.sin(angle) * 2.0
            energy_cost = 6.0
        elif action_type == ActionTypes.MOVE_LEFT:
            angle = self._agent_orientation - math.pi / 2
            move_x = math.cos(angle) * 2.0
            move_y = math.sin(angle) * 2.0
            energy_cost = 5.0
        elif action_type == ActionTypes.MOVE_RIGHT:
            angle = self._agent_orientation + math.pi / 2
            move_x = math.cos(angle) * 2.0
            move_y = math.sin(angle) * 2.0
            energy_cost = 5.0
        elif action_type == ActionTypes.TURN_LEFT:
            angular_velocity = math.pi / 4
            energy_cost = 2.0
        elif action_type == ActionTypes.TURN_RIGHT:
            angular_velocity = -math.pi / 4
            energy_cost = 2.0
        elif action_type == ActionTypes.LOOK:
            energy_cost = 1.0
        elif action_type == ActionTypes.INTERACT:
            # Try to interact with objects in range
            self._try_interact_with_objects()
            energy_cost = 3.0
        elif action_type == ActionTypes.EAT:
            # Find and consume food
            food_obj = self._find_nearest_object(WorldObjectTypes.FOOD)
            if food_obj:
                self._collect_resource(food_obj)
            energy_cost = 2.0
        elif action_type == ActionTypes.DRINK:
            # Find and consume water
            water_obj = self._find_nearest_object(WorldObjectTypes.WATER)
            if water_obj:
                self._collect_resource(water_obj)
            energy_cost = 1.5
        elif action_type == ActionTypes.REST:
            energy_cost = 0.5
            self._agent_energy = min(MAX_ENERGY, self._agent_energy + 10.0)
        elif action_type == ActionTypes.WAIT:
            energy_cost = 0.1
        
        # Apply movement
        new_x = self._agent_x + move_x
        new_y = self._agent_y + move_y
        
        # Check boundaries
        if not self._is_valid_position(new_x, new_y):
            logger.debug("Movement blocked by boundary", 
                        attempted_pos=(new_x, new_y),
                        valid_pos=(self._agent_x, self._agent_y))
        else:
            # Update position
            self._agent_x = new_x
            self._agent_y = new_y
            self._agent_is_moving = (move_x != 0 or move_y != 0)
        
        # Update orientation
        self._agent_orientation += angular_velocity * 0.1  # timestep approximation
        self._agent_is_turning = (angular_velocity != 0)
        
        # Apply energy cost
        self._agent_energy -= energy_cost
        if self._agent_energy < 0:
            self._agent_energy = 0
        
        # Update agent object
        if 'agent' in self._objects:
            agent_obj = self._objects['agent']
            agent_obj.set_position(self._agent_x, self._agent_y)
            agent_obj.velocity_x = move_x / 0.1
            agent_obj.velocity_y = move_y / 0.1
        
        self._agent_last_action_time = current_time
        
        # Create action result
        success = energy_cost > 0 and self._agent_energy >= 0
        result = ActionResult(
            reward=0.0 if success else -10.0,
            success=success,
            message=f"Action {action_type} executed"
        )
        
        logger.debug("Motor command applied",
                    action_type=action_type,
                    move=(move_x, move_y),
                    energy_cost=energy_cost,
                    new_energy=self._agent_energy,
                    success=success)
        
        return result
    
    def _try_interact_with_objects(self):
        """Try to interact with nearby objects."""
        for obj in self._world_objects:
            if obj.type in [WorldObjectTypes.RESOURCE, WorldObjectTypes.FOOD, 
                          WorldObjectTypes.WATER, WorldObjectTypes.SUNLIGHT] and \
               obj.get_distance_to(self._get_agent_object()) <= obj.radius + 1.0:
                # Interaction logic would go here
                logger.debug("Agent interacted with object", 
                            object_type=obj.type,
                            distance=obj.get_distance_to(self._get_agent_object()))
    
    def _find_nearest_object(self, object_type: str) -> Optional[WorldObject]:
        """Find nearest object of given type.
        
        Args:
            object_type: Type of object to find.
            
        Returns:
            WorldObject or None: Nearest object or None.
        """
        nearest_obj = None
        nearest_distance = float('inf')
        
        for obj in self._world_objects:
            if obj.type == object_type and obj.active:
                distance = obj.get_distance_to(self._get_agent_object())
                if distance < nearest_distance:
                    nearest_distance = distance
                    nearest_obj = obj
        
        return nearest_obj
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get sensory percept from world.
        
        Returns:
            SensoryPercept: Sensory percept data.
        """
        # Create vision data
        vision_data = self._generate_vision_data()
        
        # Create audio data (simplified)
        audio_data = self._generate_audio_data()
        
        # Create internal signals (simplified)
        internal_data = self._generate_internal_signals()
        
        # Create touch data (simplified)
        touch_data = self._generate_touch_data()
        
        # Create proprioception data (simplified)
        proprioception_data = self._generate_proprioception_data()
        
        # Create percept
        percept = SensoryPercept(
            vision=vision_data,
            audio=audio_data,
            internal=internal_data,
            touch=touch_data,
            proprioception=proprioception_data,
            timestamp=self._simulation_time
        )
        
        logger.debug("Sensory percept retrieved", 
                    vision_dimensions=vision_data.get_dimensions() if vision_data else None,
                    timestamp=self._simulation_time)
        
        return percept
    
    def _generate_vision_data(self):
        """Generate vision data based on agent position and world objects.
        
        Returns:
            Vision: Vision sensory input.
        """
        # Simplified vision generation
        vision_width = self._vision_width
        vision_height = self._vision_height
        
        # Create empty vision data
        vision = Vision(vision_width, vision_height, 3)
        
        # Fill with world objects (simplified)
        for obj in self._world_objects:
            if obj.type != WorldObjectTypes.AGENT and obj.active:
                # Calculate relative position
                rel_x = obj.x - self._agent_x
                rel_y = obj.y - self._agent_y
                
                # Convert to vision coordinates
                vision_x = int((rel_x + vision_width/2) / vision_width * vision_width)
                vision_y = int((rel_y + vision_height/2) / vision_height * vision_height)
                
                if 0 <= vision_x < vision_width and 0 <= vision_y < vision_height:
                    # Set vision pixel based on object type
                    if obj.type == WorldObjectTypes.WALL:
                        vision.set_data_at(vision_x, vision_y, 0.0, 0.0, 0.0)  # Black
                    elif obj.type == WorldObjectTypes.RESOURCE:
                        vision.set_data_at(vision_x, vision_y, 0.0, 1.0, 0.0)  # Green
                    elif obj.type == WorldObjectTypes.HAZARD:
                        vision.set_data_at(vision_x, vision_y, 1.0, 0.0, 0.0)  # Red
        
        return vision
    
    def _generate_audio_data(self):
        """Generate audio data based on environment.
        
        Returns:
            Audio: Audio sensory input.
        """
        # Simplified audio generation
        return Audio(16000, 1024)
    
    def _generate_internal_signals(self):
        """Generate internal signals based on agent state.
        
        Returns:
            InternalSignals: Internal signals.
        """
        # Simplified internal signals generation
        internal = InternalSignals()
        
        # Add agent state signals
        internal.add_signal(self._agent_energy / MAX_ENERGY)
        internal.add_signal(self._agent_health / 100.0)
        internal.add_signal(float(self._agent_age) / 1000.0)
        internal.add_signal(0.5 if self._agent_is_moving else 0.0)
        
        return internal
    
    def _generate_touch_data(self) -> List[float]:
        """Generate touch data based on agent interactions.
        
        Returns:
            List[float]: Touch data.
        """
        # Simplified touch data
        return []
    
    def _generate_proprioception_data(self) -> List[float]:
        """Generate proprioception data based on agent body.
        
        Returns:
            List[float]: Proprioception data.
        """
        # Simplified proprioception data
        return [self._agent_x / self._width, self._agent_y / self._height,
                math.sin(self._agent_orientation), math.cos(self._agent_orientation)]
    
    def get_agent_body(self) -> AgentBody:
        """Get agent body state.
        
        Returns:
            AgentBody: Agent body state.
        """
        return AgentBody(
            x=self._agent_x,
            y=self._agent_y,
            orientation=self._agent_orientation,
            velocity_x=self._agent_velocity_x,
            velocity_y=self._agent_velocity_y,
            angular_velocity=self._agent_angular_velocity,
            energy=self._agent_energy,
            health=self._agent_health,
            age=self._agent_age
        )
    
    def add_object(self, obj: WorldObject):
        """Add world object.
        
        Args:
            obj: World object to add.
        """
        if obj is None:
            raise ValueError("Object cannot be None")
        
        # Check if object name already exists
        obj_name = f"obj_{len(self._objects)}"
        while obj_name in self._objects:
            obj_name = f"obj_{len(self._objects)}"
        
        self._objects[obj_name] = obj
        self._world_objects.append(obj)
        
        logger.debug("World object added", 
                    object_type=obj.type,
                    position=(obj.x, obj.y),
                    radius=obj.radius)
    
    def remove_object(self, x: float, y: float):
        """Remove world object at position.
        
        Args:
            x: X position.
            y: Y position.
        """
        obj_to_remove = None
        
        for obj_name, obj in self._objects.items():
            if abs(obj.x - x) < 0.01 and abs(obj.y - y) < 0.01:
                obj_to_remove = obj
                break
        
        if obj_to_remove:
            self._objects.pop(list(self._objects.keys())[list(self._objects.values()).index(obj_to_remove)])
            self._world_objects.remove(obj_to_remove)
            
            logger.debug("World object removed", 
                        position=(x, y),
                        object_type=obj_to_remove.type)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid.
        
        Args:
            x: X position.
            y: Y position.
            
        Returns:
            bool: True if position valid.
        """
        return self._is_valid_position(x, y)
    
    def _is_valid_position(self, x: float, y: float) -> bool:
        """Internal position validation.
        
        Args:
            x: X position.
            y: Y position.
            
        Returns:
            bool: True if position valid.
        """
        return (0.0 <= x < self._width and 
                0.0 <= y < self._height and
                not self._is_colliding_with_walls(x, y))
    
    def _is_colliding_with_walls(self, x: float, y: float) -> bool:
        """Check if position collides with walls.
        
        Args:
            x: X position.
            y: Y position.
            
        Returns:
            bool: True if colliding with walls.
        """
        # Check for wall collisions
        for obj in self._world_objects:
            if obj.type == WorldObjectTypes.WALL and obj.is_colliding(type('Agent', (), {'x': x, 'y': y})()):
                return True
        return False
    
    def get_width(self) -> int:
        """Get world width.
        
        Returns:
            int: World width.
        """
        return self._width
    
    def get_height(self) -> int:
        """Get world height.
        
        Returns:
            int: World height.
        """
        return self._height
    
    def get_max_energy(self) -> float:
        """Get maximum agent energy.
        
        Returns:
            float: Maximum energy.
        """
        return MAX_ENERGY
    
    def set_max_energy(self, energy: float):
        """Set maximum agent energy.
        
        Args:
            energy: Maximum energy.
        """
        if energy <= 0:
            raise ValueError("Max energy must be positive")
        # Note: This only affects new agents, not current agent
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            float: Energy decay rate.
        """
        return ENERGY_DECAY_RATE
    
    def set_energy_decay_rate(self, rate: float):
        """Set energy decay rate.
        
        Args:
            rate: Energy decay rate.
        """
        if rate < 0:
            raise ValueError("Energy decay rate cannot be negative")
        # Note: This only affects new agents, not current agent
    
    def get_simulation_time(self) -> float:
        """Get simulation time.
        
        Returns:
            float: Simulation time.
        """
        return self._simulation_time
    
    def set_random_seed(self, seed: int):
        """Set random seed for world.
        
        Args:
            seed: Random seed.
        """
        # Note: Simplified implementation
        pass
    
    def get_random_seed(self) -> int:
        """Get random seed for world.
        
        Returns:
            int: Random seed.
        """
        return 42  # Default seed
    
    def get_agent_object(self) -> WorldObject:
        """Get agent object.
        
        Returns:
            WorldObject: Agent object.
        """
        return self._get_agent_object()
    
    def _get_agent_object(self) -> WorldObject:
        """Get agent object (internal)."""
        return self._objects.get('agent')
    
    def get_objects_in_range(self, x: float, y: float, range_distance: float) -> List[WorldObject]:
        """Get objects within range of position.
        
        Args:
            x: X position.
            y: Y position.
            range_distance: Range distance.
            
        Returns:
            List[WorldObject]: Objects within range.
        """
        objects_in_range = []
        
        for obj in self._world_objects:
            if obj.is_in_range(x, y, range_distance):
                objects_in_range.append(obj)
        
        return objects_in_range
    
    def get_objects_by_type(self, object_type: str) -> List[WorldObject]:
        """Get objects by type.
        
        Args:
            object_type: Object type.
            
        Returns:
            List[WorldObject]: Objects of specified type.
        """
        return [obj for obj in self._world_objects if obj.type == object_type]


# Helper functions

def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world.
    
    Returns:
        SimpleWorld: New simple world.
    """
    logger.debug("Creating simple world")
    return SimpleWorld()


def createWorld(width: int = DEFAULT_WORLD_WIDTH, height: int = DEFAULT_WORLD_HEIGHT,
               vision_width: int = DEFAULT_VISION_WIDTH, 
               vision_height: int = DEFAULT_VISION_HEIGHT) -> SimpleWorld:
    """Create a new world with specified dimensions.
    
    Args:
        width: World width.
        height: World height.
        vision_width: Vision width.
        vision_height: Vision height.
        
    Returns:
        SimpleWorld: New world.
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world


def get_default_world_objects() -> Dict[str, Any]:
    """Get default world objects for testing.
    
    Returns:
        Dict[str, Any]: Default world objects.
    """
    return {
        'agent': WorldObject(0.0, 0.0, WorldObjectTypes.AGENT),
        'food': WorldObject(5.0, 5.0, WorldObjectTypes.FOOD),
        'water': WorldObject(8.0, 3.0, WorldObjectTypes.WATER),
        'wall': WorldObject(2.0, 2.0, WorldObjectTypes.WALL, radius=1.0),
        'hazard': WorldObject(7.0, 7.0, WorldObjectTypes.HAZARD, radius=1.5)
    }


# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Export all public symbols
__all__ = [
    # Classes
    'SimpleWorld',
    'WorldObject',
    'Vector2D',
    
    # Types
    'WorldObjectTypes',
    
    # Exception classes
    'WorldError',
    'InvalidPositionError',
    'WorldConfigurationError',
    
    # Helper functions
    'createSimpleWorld',
    'createWorld',
    'get_default_world_objects',
]
