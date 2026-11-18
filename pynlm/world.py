'''Simple world class for pynlm'''

from __future__ import annotations

from typing import Dict, List, Optional, Any
from .core.types import NeuronId, SynapseId, PopulationId, SimulationStep, Timestamp
from .core.enums import WorldObjectType, ActionType
from .agent.AgentBody import AgentBody
from .agent.SensoryPercept import SensoryPercept


class SimpleWorld:
    """Simple 2D world for NLM simulation.
    
    The SimpleWorld class represents a simulated environment where the agent
    interacts with objects, senses its surroundings, and performs actions.
    It provides basic physics simulation and perception mechanisms.
    """
    
    def __init__(self):
        """Initialize a SimpleWorld instance."""
        self.width: float = 0.0
        self.height: float = 0.0
        self.vision_width: float = 0.0
        self.vision_height: float = 0.0
        self.max_energy: float = 1.0
        self.energy_decay_rate: float = 0.0
        self.random_seed: Optional[int] = None
        
        # Environment state
        self.agent: Optional[AgentBody] = None
        self.objects: Dict[str, Any] = {}
        self.sensory_percept: Optional[SensoryPercept] = None
        
        # Physics state
        self.simulation_time: Timestamp = 0.0
        self.simulation_step: SimulationStep = 0
        
        # Simulation parameters
        self.enable_reward: bool = True
        self.enable_movement: bool = True
        self.enable_collisions: bool = True
        
        # Statistics
        self.total_steps: SimulationStep = 0
        
        """Default configuration values."""
        # Default configuration
        self.default_width = 20.0
        self.default_height = 15.0
        self.default_vision_width = 10
        self.default_vision_height = 10
        self.default_max_energy = 100.0
        self.default_energy_decay_rate = 0.01
        self.default_max_speed = 5.0
        self.default_turn_speed = 0.1
    
    def configure(self, width: float, height: float, 
                 vision_width: int, vision_height: int):
        """Configure the world dimensions and vision.
        
        Args:
            width: World width in arbitrary units.
            height: World height in arbitrary units.
            vision_width: Width of agent's vision field in pixels.
            vision_height: Height of agent's vision field in pixels.
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Initialize sensory percept with vision dimensions
        self.sensory_percept = SensoryPercept(vision_width, vision_height)
        
        self._validate_configuration()
    
    def _validate_configuration(self):
        """Validate world configuration parameters."""
        if self.width <= 0 or self.height <= 0:
            raise ValueError("World dimensions must be positive")
        
        if self.vision_width <= 0 or self.vision_height <= 0:
            raise ValueError("Vision dimensions must be positive")
        
        if self.vision_width > self.width or self.vision_height > self.height:
            raise ValueError("Vision dimensions cannot exceed world dimensions")
    
    def reset(self):
        """Reset world state to initial conditions."""
        self.simulation_time = 0.0
        self.simulation_step = 0
        self.total_steps = 0
        
        if self.agent:
            self.agent.reset()
        
        # Clear objects (except special persistent objects)
        objects_to_clear = [obj_id for obj_id, obj in self.objects.items() 
                          if obj.get('persistent', False)]
        self.objects.clear()
        
        for obj_id in objects_to_clear:
            self.objects[obj_id] = objects_to_clear[obj_id]
        
        self.sensory_percept = SensoryPercept(self.vision_width, self.vision_height)
    
    def setAgentStart(self, x: float, y: float):
        """Set the agent's starting position in the world.
        
        Args:
            x: X-coordinate for agent start position.
            y: Y-coordinate for agent start position.
        """
        if not self._is_valid_position(x, y):
            raise ValueError(f"Invalid position ({x}, {y}) for world of size "
                           f"({self.width}, {self.height})")
        
        if self.agent is None:
            self.agent = AgentBody()
        
        self.agent.x = x
        self.agent.y = y
        self.agent.isMoving = False
        self.agent.isTurning = False
        self.agent.lastActionTime = self.simulation_time
    
    def update(self, timestep: float):
        """Update world state for one timestep.
        
        Args:
            timestep: Time step duration in seconds.
        """
        # Update agent physics
        if self.agent and self.enable_movement:
            self._update_agent_physics(timestep)
        
        # Update energy decay
        if self.agent:
            self.agent.energy -= self.energy_decay_rate * timestep
            self.agent.energy = max(0.0, self.agent.energy)
        
        # Update sensory input
        if self.sensory_percept:
            self._update_sensory_input()
        
        # Update simulation counters
        self.simulation_step += 1
        self.simulation_time += timestep
        self.total_steps += 1
    
    def _update_agent_physics(self, timestep: float):
        """Update agent physics and position.
        
        Args:
            timestep: Time step duration in seconds.
        """
        # Simple Euler integration for position
        if self.agent.isMoving:
            # Update position based on velocity
            self.agent.x += self.agent.velocityX * timestep
            self.agent.y += self.agent.velocityY * timestep
            
            # Update orientation based on angular velocity
            self.agent.orientation += self.agent.angularVelocity * timestep
        
        # Clamp position to world bounds
        self.agent.x = max(0.0, min(self.agent.x, self.width))
        self.agent.y = max(0.0, min(self.agent.y, self.height))
        
        # Update age
        self.agent.age += timestep
        
        # Check for collisions with objects
        if self.enable_collisions:
            self._check_collisions()
    
    def _check_collisions(self):
        """Check for collisions between agent and objects."""
        if not self.agent:
            return
        
        for obj_id, obj in self.objects.items():
            if not obj.get('active', True):
                continue
            
            obj_type = obj.get('type', WorldObjectType.EMPTY)
            if obj_type == WorldObjectType.WALL:
                # Check wall collision
                obj_x = obj.get('x', 0.0)
                obj_y = obj.get('y', 0.0)
                obj_radius = obj.get('radius', 0.5)
                
                distance = ((self.agent.x - obj_x) ** 2 + 
                          (self.agent.y - obj_y) ** 2) ** 0.5
                
                if distance < obj_radius + 0.5:  # Agent radius ~0.5
                    # Resolve collision (simple elastic bounce)
                    dx = obj_x - self.agent.x
                    dy = obj_y - self.agent.y
                    dist = (dx ** 2 + dy ** 2) ** 0.5
                    
                    if dist > 0:
                        dx /= dist
                        dy /= dist
                        
                        # Push agent away from wall
                        overlap = (obj_radius + 0.5) - distance
                        self.agent.x += dx * overlap
                        self.agent.y += dy * overlap
                        
                        # Reverse velocity component
                        dot_product = (self.agent.velocityX * dx + 
                                     self.agent.velocityY * dy)
                        self.agent.velocityX -= 2 * dot_product * dx
                        self.agent.velocityY -= 2 * dot_product * dy
    
    def _update_sensory_input(self):
        """Update sensory input based on agent position and environment."""
        if not self.sensory_percept or not self.agent:
            return
        
        # Clear existing percepts
        self.sensory_percept.clearAllSignals()
        
        # Process world objects for perception
        for obj_id, obj in self.objects.items():
            if not obj.get('active', True):
                continue
            
            obj_x = obj.get('x', 0.0)
            obj_y = obj.get('y', 0.0)
            obj_type = obj.get('type', WorldObjectType.EMPTY)
            obj_radius = obj.get('radius', 0.5)
            
            # Calculate relative position and distance
            dx = obj_x - self.agent.x
            dy = obj_y - self.agent.y
            distance = (dx ** 2 + dy ** 2) ** 0.5
            
            # Add to vision if within vision range
            if distance < self.vision_width / 2:
                # Normalize to vision coordinates
                rel_x = (dx / (self.vision_width / 2)) * self.vision_width
                rel_y = (dy / (self.vision_height / 2)) * self.vision_height
                
                # Clamp to vision dimensions
                rel_x = max(0, min(rel_x, self.vision_width - 1))
                rel_y = max(0, min(rel_y, self.vision_height - 1))
                
                # Add signal based on object type
                if obj_type == WorldObjectType.RESOURCE:
                    signal_value = obj.get('value', 1.0)
                    self.sensory_percept.addVisionSignal(rel_x, rel_y, signal_value)
                elif obj_type == WorldObjectType.HAZARD:
                    signal_value = -obj.get('value', 1.0)
                    self.sensory_percept.addVisionSignal(rel_x, rel_y, signal_value)
                elif obj_type == WorldObjectType.MARKER:
                    signal_value = 2.0  # Special marker signal
                    self.sensory_percept.addVisionSignal(rel_x, rel_y, signal_value)
    
    def applyMotorCommand(self, cmd: int, current_time: Timestamp):
        """Apply a motor command to the agent.
        
        Args:
            cmd: Motor command to apply.
            current_time: Current simulation time.
        """
        if not self.agent:
            return
        
        # Process command based on type
        if cmd == 0:  # MoveForward
            self.agent.isMoving = True
            speed = self._get_speed_for_command(cmd)
            self.agent.velocityX += speed * timestep
            
        elif cmd == 1:  # MoveBackward
            self.agent.isMoving = True
            speed = self._get_speed_for_command(cmd)
            self.agent.velocityX -= speed * timestep
            
        elif cmd == 2:  # TurnLeft
            self.agent.isTurning = True
            self.agent.angularVelocity -= self._get_turn_speed_for_command(cmd) * timestep
            
        elif cmd == 3:  # TurnRight
            self.agent.isTurning = True
            self.agent.angularVelocity += self._get_turn_speed_for_command(cmd) * timestep
            
        elif cmd == 4:  # LookLeft
            # This would affect sensor direction in a real implementation
            pass
            
        elif cmd == 5:  # LookRight
            # This would affect sensor direction in a real implementation
            pass
            
        elif cmd == 6:  # Interact
            self._handle_interaction()
            
        elif cmd == 7:  # Wait
            # No movement
            pass
        
        # Clamp velocities
        speed_limit = self._get_speed_limit()
        velocity_magnitude = (self.agent.velocityX ** 2 + self.agent.velocityY ** 2) ** 0.5
        if velocity_magnitude > speed_limit:
            scale = speed_limit / velocity_magnitude
            self.agent.velocityX *= scale
            self.agent.velocityY *= scale
        
        self.agent.angularVelocity = max(-self._get_turn_limit(), 
                                        min(self._get_turn_limit(), self.agent.angularVelocity))
        
        self.agent.lastActionTime = current_time
    
    def _get_speed_for_command(self, cmd: int) -> float:
        """Get speed for a motor command.
        
        Args:
            cmd: Motor command.
            
        Returns:
            Speed value for the command.
        """
        return self.default_max_speed * 0.8
    
    def _get_turn_speed_for_command(self, cmd: int) -> float:
        """Get turn speed for a motor command.
        
        Args:
            cmd: Motor command.
            
        Returns:
            Turn speed for the command.
        """
        return self.default_turn_speed
    
    def _get_speed_limit(self) -> float:
        """Get maximum speed limit.
        
        Returns:
            Maximum speed limit.
        """
        return self.default_max_speed
    
    def _get_turn_limit(self) -> float:
        """Get maximum turn speed limit.
        
        Returns:
            Maximum turn speed limit.
        """
        return 0.5
    
    def _handle_interaction(self):
        """Handle interaction with nearby objects."""
        if not self.agent:
            return
        
        # Find closest object
        closest_obj = None
        min_distance = float('inf')
        
        for obj_id, obj in self.objects.items():
            if not obj.get('active', True):
                continue
            
            obj_x = obj.get('x', 0.0)
            obj_y = obj.get('y', 0.0)
            distance = ((self.agent.x - obj_x) ** 2 + 
                      (self.agent.y - obj_y) ** 2) ** 0.5
            
            if distance < min_distance:
                min_distance = distance
                closest_obj = obj
        
        # Interact if close enough
        if closest_obj and min_distance < 1.0:  # Interaction radius
            obj_type = closest_obj.get('type', WorldObjectType.EMPTY)
            
            if obj_type == WorldObjectType.RESOURCE:
                # Consume resource
                closest_obj['value'] = max(0.0, closest_obj['value'] - 10.0)
                if closest_obj['value'] <= 0:
                    closest_obj['active'] = False
                self.agent.energy = min(self.agent.energy + 10.0, self.agent.energy + 100.0)
            
            elif obj_type == WorldObjectType.HAZARD:
                # Take damage
                self.agent.health = max(0.0, self.agent.health - 10.0)
    
    def getSensoryPercept(self) -> Optional[SensoryPercept]:
        """Get current sensory percept.
        
        Returns:
            Current sensory percept object.
        """
        return self.sensory_percept
    
    def getAgentBody(self) -> Optional[AgentBody]:
        """Get agent body state.
        
        Returns:
            Current agent body state.
        """
        return self.agent
    
    def addObject(self, obj: Any):
        """Add an object to the world.
        
        Args:
            obj: Object to add. Should be a dict with x, y coordinates
                 and type information.
        """
        if 'x' not in obj or 'y' not in obj or 'type' not in obj:
            raise ValueError("Object must have x, y, and type fields")
        
        obj_id = f"obj_{len(self.objects)}"
        obj['active'] = True
        self.objects[obj_id] = obj
    
    def removeObject(self, x: float, y: float):
        """Remove an object at the specified position.
        
        Args:
            x: X-coordinate of object to remove.
            y: Y-coordinate of object to remove.
        """
        for obj_id, obj in list(self.objects.items()):
            if (abs(obj.get('x', 0.0) - x) < 0.1 and 
                abs(obj.get('y', 0.0) - y) < 0.1):
                obj['active'] = False
                # Optionally remove from dict
                # del self.objects[obj_id]
                break
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if a position is valid in the world.
        
        Args:
            x: X-coordinate to check.
            y: Y-coordinate to check.
            
        Returns:
            True if position is valid, False otherwise.
        """
        if x < 0 or x > self.width or y < 0 or y > self.height:
            return False
        
        # Check for wall collisions
        for obj in self.objects.values():
            if not obj.get('active', True):
                continue
            
            obj_x = obj.get('x', 0.0)
            obj_y = obj.get('y', 0.0)
            obj_radius = obj.get('radius', 0.5)
            
            distance = ((x - obj_x) ** 2 + (y - obj_y) ** 2) ** 0.5
            
            if distance < obj_radius + 0.5:  # Agent radius ~0.5
                return False
        
        return True
    
    def getWidth(self) -> float:
        """Get world width.
        
        Returns:
            World width.
        """
        return self.width
    
    def getHeight(self) -> float:
        """Get world height.
        
        Returns:
            World height.
        """
        return self.height
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy.
        
        Returns:
            Maximum energy.
        """
        return self.max_energy
    
    def setMaxEnergy(self, e: float):
        """Set maximum energy.
        
        Args:
            e: New maximum energy.
        """
        self.max_energy = e
        if self.agent:
            self.agent.energy = min(self.agent.energy, e)
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            Energy decay rate.
        """
        return self.energy_decay_rate
    
    def setEnergyDecayRate(self, r: float):
        """Set energy decay rate.
        
        Args:
            r: New energy decay rate.
        """
        self.energy_decay_rate = r
    
    def getSimulationTime(self) -> Timestamp:
        """Get current simulation time.
        
        Returns:
            Current simulation time.
        """
        return self.simulation_time
    
    def setRandomSeed(self, seed: int):
        """Set random seed for reproducible simulations.
        
        Args:
            seed: Random seed value.
        """
        self.random_seed = seed
        # In a real implementation, this would seed random number generators
    
    def getRandomSeed(self) -> Optional[int]:
        """Get current random seed.
        
        Returns:
            Current random seed.
        """
        return self.random_seed
    
    def __repr__(self) -> str:
        """Get string representation of the world.
        
        Returns:
            String representation of the world.
        """
        return f"<SimpleWorld (width: {self.width}, height: {self.height}, " \
               f"vision: {self.vision_width}x{self.vision_height}, " \
               f"step: {self.simulation_step}, time: {self.simulation_time:.2f}s)>"