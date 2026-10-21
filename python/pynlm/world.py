"""
Python bindings for NLM world class.

This module provides Python bindings for the NLM world class,
which represents the environment in which the agent operates.
"""

from .core import (
    SensoryPercept, Vision, Audio, InternalSignals, 
    WorldObject, WorldObjectType, Action, ActionType, AgentBody, ActionResult
)
class SimpleWorld:
    """
    Simple 2D world for NLM simulation.
    
    This class represents a 2D grid world where the agent can move,
    interact with objects, and receive sensory input.
    """
    
    def __init__(self):
        """Initialize the world."""
        self._width = 0
        self._height = 0
        self._vision_width = 0
        self._vision_height = 0
        self._vision_channels = 3
        
        # World state
        self._objects = {}  # {(x, y): WorldObject}
        self._agent = AgentBody()
        self._simulation_time = 0.0
        self._random_seed = 42
        
        # Vision and perception
        self._percept = SensoryPercept()
        
        # World parameters
        self._max_energy = 100.0
        self._energy_decay_rate = 0.01
        
        # Performance optimizations
        self._vision_cache = {}
    
    def configure(self, width: int, height: int, 
                  visionWidth: int, visionHeight: int, 
                  visionChannels: int = 3):
        """
        Configure the world.
        
        Args:
            width: World width
            height: World height
            visionWidth: Agent's vision width
            visionHeight: Agent's vision height
            visionChannels: Number of vision channels
        """
        self._width = width
        self._height = height
        self._vision_width = visionWidth
        self._vision_height = visionHeight
        self._vision_channels = visionChannels
        
        # Reset agent position
        self._agent.x = width // 2
        self._agent.y = height // 2
        self._agent.orientation = 0.0
        
        # Clear objects
        self._objects.clear()
        
        # Update vision cache
        self._vision_cache.clear()
    
    def reset(self):
        """Reset the world to initial state."""
        self._agent.reset()
        self._objects.clear()
        self._simulation_time = 0.0
        self._vision_cache.clear()
        
        # Create default environment
        self._create_default_environment()
    
    def _create_default_environment(self):
        """Create the default environment."""
        # Add some walls
        for x in range(self._width):
            self._add_object(x, 0, WorldObjectType.Wall, 1.0, 0.5)
            self._add_object(x, self._height - 1, WorldObjectType.Wall, 1.0, 0.5)
        
        for y in range(self._height):
            self._add_object(0, y, WorldObjectType.Wall, 1.0, 0.5)
            self._add_object(self._width - 1, y, WorldObjectType.Wall, 1.0, 0.5)
    
    def _add_object(self, x: float, y: float, obj_type: WorldObjectType, 
                   value: float = 0.0, radius: float = 0.5):
        """Add an object to the world."""
        obj = WorldObject(x, y, obj_type, value, radius)
        self._objects[(int(x), int(y))] = obj
    
    def setAgentStart(self, x: float, y: float):
        """
        Set agent start position.
        
        Args:
            x: Start X coordinate
            y: Start Y coordinate
        """
        self._agent.x = x
        self._agent.y = y
    
    def update(self, timestep: float):
        """
        Update the world by timestep.
        
        Args:
            timestep: Time step to update
        """
        self._simulation_time += timestep
        
        # Update agent state
        self._update_agent(timestep)
        
        # Decay agent energy
        self._agent.energy *= (1.0 - self._energy_decay_rate * timestep)
        
        # Update agent age
        self._agent.age += int(timestep * 10)  # 10 ticks per second
        
        # Generate percept
        self._generate_percept()
    
    def _update_agent(self, timestep: float):
        """Update agent state based on velocity and actions."""
        if self._agent.isMoving:
            # Update position based on velocity
            self._agent.x += self._agent.velocityX * timestep
            self._agent.y += self._agent.velocityY * timestep
            
            # Update orientation based on angular velocity
            self._agent.orientation += self._agent.angularVelocity * timestep
        
        # Clamp position to world bounds
        self._agent.x = max(0.0, min(self._width - 1.0, self._agent.x))
        self._agent.y = max(0.0, min(self._height - 1.0, self._agent.y))
    
    def _generate_percept(self):
        """Generate sensory percept for the agent."""
        # Generate vision
        self._generate_vision()
        
        # Generate internal signals
        internal = InternalSignals()
        internal.addSignal(self._agent.energy)
        internal.addSignal(self._agent.health)
        internal.addSignal(self._agent.age)
        self._percept.setInternal(internal)
        
        # Generate proprioception
        proprioception = InternalSignals()
        proprioception.addSignal(self._agent.velocityX)
        proprioception.addSignal(self._agent.velocityY)
        proprioception.addSignal(self._agent.angularVelocity)
        self._percept.setProprioception(proprioception)
        
        self._percept.setTimestamp(self._simulation_time)
    
    def _generate_vision(self):
        """Generate vision percept for the agent."""
        # Get vision area centered on agent
        vision_data = []
        
        # Simple vision sampling (ray casting would be more realistic)
        for y in range(-self._vision_height // 2, self._vision_height // 2 + 1):
            row = []
            for x in range(-self._vision_width // 2, self._vision_width // 2 + 1):
                world_x = self._agent.x + x
                world_y = self._agent.y + y
                
                if (0 <= world_x < self._width and 0 <= world_y < self._height):
                    obj = self._objects.get((int(world_x), int(world_y)))
                    if obj and obj.active:
                        # Encode object type
                        obj_type = obj.type
                        if obj_type == WorldObjectType.Empty:
                            row.append(0.0)
                        elif obj_type == WorldObjectType.Resource:
                            row.append(1.0)
                        elif obj_type == WorldObjectType.Hazard:
                            row.append(-1.0)
                        elif obj_type == WorldObjectType.Wall:
                            row.append(-0.5)
                        else:
                            row.append(0.0)
                    else:
                        row.append(0.0)  # Empty space
                else:
                    row.append(-2.0)  # Out of bounds
            
            vision_data.extend(row)
        
        # Create vision object
        vision = Vision(self._vision_width, self._vision_height, self._vision_channels)
        vision.setData(vision_data)
        self._percept.setVision(vision)
    
    def applyMotorCommand(self, action: Action, currentTime: float):
        """
        Apply motor command to the world.
        
        Args:
            action: Motor command to apply
            currentTime: Current simulation time
        """
        self._agent.lastActionTime = currentTime
        
        # Execute action
        if action.getType() == ActionType.MoveForward:
            self._agent.isMoving = True
            self._agent.velocityX = 1.0
        elif action.getType() == ActionType.MoveBackward:
            self._agent.isMoving = True
            self._agent.velocityX = -1.0
        elif action.getType() == ActionType.TurnLeft:
            self._agent.isTurning = True
            self._agent.angularVelocity = -1.0
        elif action.getType() == ActionType.TurnRight:
            self._agent.isTurning = True
            self._agent.angularVelocity = 1.0
        elif action.getType() == ActionType.Look:
            pass  # Already handled by vision generation
        elif action.getType() == ActionType.Interact:
            self._try_interact()
        elif action.getType() == ActionType.Rest:
            self._agent.velocityX = 0.0
            self._agent.velocityY = 0.0
            self._agent.angularVelocity = 0.0
            self._agent.isMoving = False
            self._agent.isTurning = False
        else:
            # Unknown action - stop
            self._agent.velocityX = 0.0
            self._agent.velocityY = 0.0
            self._agent.angularVelocity = 0.0
            self._agent.isMoving = False
            self._agent.isTurning = False
        
        # Apply action result effects
        result = self._execute_action_effects(action)
        if result.success:
            self._agent.energy = max(0.0, self._agent.energy - 1.0)
            if result.message:
                print(f"Action result: {result.message}")
    
    def _try_interact(self):
        """Try to interact with objects in the world."""
        # Check for nearby objects
        for (x, y), obj in self._objects.items():
            if obj and obj.active:
                distance = ((x - self._agent.x) ** 2 + (y - self._agent.y) ** 2) ** 0.5
                if distance <= 1.5:  # Interaction radius
                    if obj.type == WorldObjectType.Resource:
                        # Collect resource
                        obj.active = False
                        del self._objects[(x, y)]
                        return True
                    elif obj.type == WorldObjectType.Hazard:
                        # Hazard damage
                        self._agent.health = max(0.0, self._agent.health - 10.0)
                        return True
        return False
    
    def _execute_action_effects(self, action: Action) -> ActionResult:
        """Execute action-specific effects and return result."""
        # Default successful action
        return ActionResult(reward=1.0, success=True, message="Action completed")
    
    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept."""
        return self._percept
    
    def getAgentBody(self) -> AgentBody:
        """Get agent body state."""
        return self._agent
    
    def addObject(self, obj: WorldObject):
        """
        Add an object to the world.
        
        Args:
            obj: World object to add
        """
        self._objects[(int(obj.x), int(obj.y))] = obj
    
    def removeObject(self, x: float, y: float):
        """
        Remove an object from the world.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        key = (int(x), int(y))
        if key in self._objects:
            del self._objects[key]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """
        Check if position is valid.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid, False otherwise
        """
        return (0 <= x < self._width and 0 <= y < self._height)
    
    def getWidth(self) -> int:
        """Get world width."""
        return self._width
    
    def getHeight(self) -> int:
        """Get world height."""
        return self._height
    
    def getMaxEnergy(self) -> float:
        """Get maximum agent energy."""
        return self._max_energy
    
    def setMaxEnergy(self, energy: float):
        """
        Set maximum agent energy.
        
        Args:
            energy: Maximum energy
        """
        self._max_energy = energy
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate."""
        return self._energy_decay_rate
    
    def setEnergyDecayRate(self, rate: float):
        """
        Set energy decay rate.
        
        Args:
            rate: Energy decay rate
        """
        self._energy_decay_rate = rate
    
    def getSimulationTime(self) -> float:
        """Get current simulation time."""
        return self._simulation_time
    
    def setRandomSeed(self, seed: int):
        """
        Set random seed.
        
        Args:
            seed: Random seed
        """
        self._random_seed = seed
    
    def getRandomSeed(self) -> int:
        """Get random seed."""
        return self._random_seed
