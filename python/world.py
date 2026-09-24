"""
World interface for NLM - environment and agent interaction.

This module provides the world interface that defines the environment
in which the NLM agent operates and the interaction between the agent
and environment.
"""

import sys
from typing import List, Dict, Any, Optional

__all__ = [
    'SimpleWorld',
    'WorldObject',
    'AgentBody',
]


class WorldObject:
    """Object in the NLM world.
    
    This class represents an object in the simulated environment that
    the agent can interact with. Objects have position, type, value, and
    activation status.
    
    Attributes:
        x: X coordinate of object.
        y: Y coordinate of object.
        type: Object type (e.g., "Empty", "Resource", "Hazard").
        value: Object value (e.g., nutritional value, hazard level).
        radius: Object radius.
        active: Whether object is active.
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: str = "Empty",
                 value: float = 0.0, radius: float = 0.5, active: bool = True):
        self._x = x
        self._y = y
        self._type = obj_type
        self._value = value
        self._radius = radius
        self._active = active
    
    def getX(self) -> float:
        """Get object X coordinate.
        
        Returns:
            float: X coordinate.
        """
        return self._x
    
    def setX(self, x: float) -> bool:
        """Set object X coordinate.
        
        Args:
            x: New X coordinate.
            
        Returns:
            bool: True if coordinate set successfully.
        """
        self._x = x
        return True
    
    def getY(self) -> float:
        """Get object Y coordinate.
        
        Returns:
            float: Y coordinate.
        """
        return self._y
    
    def setY(self, y: float) -> bool:
        """Set object Y coordinate.
        
        Args:
            y: New Y coordinate.
            
        Returns:
            bool: True if coordinate set successfully.
        """
        self._y = y
        return True
    
    def getType(self) -> str:
        """Get object type.
        
        Returns:
            str: Object type.
        """
        return self._type
    
    def setType(self, obj_type: str) -> bool:
        """Set object type.
        
        Args:
            obj_type: New object type.
            
        Returns:
            bool: True if type set successfully.
        """
        self._type = obj_type
        return True
    
    def getValue(self) -> float:
        """Get object value.
        
        Returns:
            float: Object value.
        """
        return self._value
    
    def setValue(self, value: float) -> bool:
        """Set object value.
        
        Args:
            value: New object value.
            
        Returns:
            bool: True if value set successfully.
        """
        self._value = value
        return True
    
    def getRadius(self) -> float:
        """Get object radius.
        
        Returns:
            float: Object radius.
        """
        return self._radius
    
    def setRadius(self, radius: float) -> bool:
        """Set object radius.
        
        Args:
            radius: New object radius.
            
        Returns:
            bool: True if radius set successfully.
        """
        self._radius = radius
        return True
    
    def getActive(self) -> bool:
        """Get object active status.
        
        Returns:
            bool: Whether object is active.
        """
        return self._active
    
    def setActive(self, active: bool) -> bool:
        """Set object active status.
        
        Args:
            active: New active status.
            
        Returns:
            bool: True if active status set successfully.
        """
        self._active = active
        return True
    
    def distanceTo(self, other: 'WorldObject') -> float:
        """Calculate distance to another object.
        
        Args:
            other: Other world object.
            
        Returns:
            float: Distance between objects.
        """
        dx = self._x - other.getX()
        dy = self._y - other.getY()
        return (dx * dx + dy * dy) ** 0.5
    
    def __repr__(self) -> str:
        return f"<WorldObject: type={self._type}, x={self._x:.1f}, y={self._y:.1f}, active={self._active}>"


class AgentBody:
    """Agent body state in the NLM world.
    
    This class represents the agent's body state, including position,
    orientation, velocity, energy, and health. It tracks the agent's
    physical state in the simulated environment.
    
    Attributes:
        x: X coordinate of agent.
        y: Y coordinate of agent.
        orientation: Agent orientation (angle in radians).
        velocityX: X velocity.
        velocityY: Y velocity.
        angularVelocity: Angular velocity.
        energy: Current energy level.
        health: Current health level.
        age: Agent age (simulation steps).
        isMoving: Whether agent is currently moving.
        isTurning: Whether agent is currently turning.
        lastActionTime: Time since last action.
    """
    
    def __init__(self):
        self._x = 0.0
        self._y = 0.0
        self._orientation = 0.0
        self._velocityX = 0.0
        self._velocityY = 0.0
        self._angularVelocity = 0.0
        self._energy = 100.0
        self._health = 100.0
        self._age = 0
        self._isMoving = False
        self._isTurning = False
        self._lastActionTime = 0.0
    
    def getX(self) -> float:
        """Get agent X coordinate.
        
        Returns:
            float: X coordinate.
        """
        return self._x
    
    def setX(self, x: float) -> bool:
        """Set agent X coordinate.
        
        Args:
            x: New X coordinate.
            
        Returns:
            bool: True if coordinate set successfully.
        """
        self._x = x
        return True
    
    def getY(self) -> float:
        """Get agent Y coordinate.
        
        Returns:
            float: Y coordinate.
        """
        return self._y
    
    def setY(self, y: float) -> bool:
        """Set agent Y coordinate.
        
        Args:
            y: New Y coordinate.
            
        Returns:
            bool: True if coordinate set successfully.
        """
        self._y = y
        return True
    
    def getOrientation(self) -> float:
        """Get agent orientation.
        
        Returns:
            float: Orientation in radians.
        """
        return self._orientation
    
    def setOrientation(self, orientation: float) -> bool:
        """Set agent orientation.
        
        Args:
            orientation: New orientation (radians).
            
        Returns:
            bool: True if orientation set successfully.
        """
        self._orientation = orientation
        return True
    
    def getVelocityX(self) -> float:
        """Get agent X velocity.
        
        Returns:
            float: X velocity.
        """
        return self._velocityX
    
    def setVelocityX(self, velocityX: float) -> bool:
        """Set agent X velocity.
        
        Args:
            velocityX: New X velocity.
            
        Returns:
            bool: True if velocity set successfully.
        """
        self._velocityX = velocityX
        return True
    
    def getVelocityY(self) -> float:
        """Get agent Y velocity.
        
        Returns:
            float: Y velocity.
        """
        return self._velocityY
    
    def setVelocityY(self, velocityY: float) -> bool:
        """Set agent Y velocity.
        
        Args:
            velocityY: New Y velocity.
            
        Returns:
            bool: True if velocity set successfully.
        """
        self._velocityY = velocityY
        return True
    
    def getAngularVelocity(self) -> float:
        """Get agent angular velocity.
        
        Returns:
            float: Angular velocity.
        """
        return self._angularVelocity
    
    def setAngularVelocity(self, angularVelocity: float) -> bool:
        """Set agent angular velocity.
        
        Args:
            angularVelocity: New angular velocity.
            
        Returns:
            bool: True if angular velocity set successfully.
        """
        self._angularVelocity = angularVelocity
        return True
    
    def getEnergy(self) -> float:
        """Get agent energy.
        
        Returns:
            float: Current energy (0.0 to 100.0).
        """
        return self._energy
    
    def setEnergy(self, energy: float) -> bool:
        """Set agent energy.
        
        Args:
            energy: New energy (0.0 to 100.0).
            
        Returns:
            bool: True if energy set successfully.
        """
        self._energy = max(0.0, min(100.0, energy))
        return True
    
    def getHealth(self) -> float:
        """Get agent health.
        
        Returns:
            float: Current health (0.0 to 100.0).
        """
        return self._health
    
    def setHealth(self, health: float) -> bool:
        """Set agent health.
        
        Args:
            health: New health (0.0 to 100.0).
            
        Returns:
            bool: True if health set successfully.
        """
        self._health = max(0.0, min(100.0, health))
        return True
    
    def getAge(self) -> int:
        """Get agent age.
        
        Returns:
            int: Agent age in simulation steps.
        """
        return self._age
    
    def setAge(self, age: int) -> bool:
        """Set agent age.
        
        Args:
            age: New age.
            
        Returns:
            bool: True if age set successfully.
        """
        self._age = age
        return True
    
    def isMoving(self) -> bool:
        """Check if agent is moving.
        
        Returns:
            bool: Whether agent is moving.
        """
        return self._isMoving
    
    def setMoving(self, moving: bool) -> bool:
        """Set agent moving status.
        
        Args:
            moving: New moving status.
            
        Returns:
            bool: True if moving status set successfully.
        """
        self._isMoving = moving
        return True
    
    def isTurning(self) -> bool:
        """Check if agent is turning.
        
        Returns:
            bool: Whether agent is turning.
        """
        return self._isTurning
    
    def setTurning(self, turning: bool) -> bool:
        """Set agent turning status.
        
        Args:
            turning: New turning status.
            
        Returns:
            bool: True if turning status set successfully.
        """
        self._isTurning = turning
        return True
    
    def getLastActionTime(self) -> float:
        """Get time since last action.
        
        Returns:
            float: Time since last action.
        """
        return self._lastActionTime
    
    def setLastActionTime(self, time: float) -> bool:
        """Set time since last action.
        
        Args:
            time: New last action time.
            
        Returns:
            bool: True if time set successfully.
        """
        self._lastActionTime = time
        return True
    
    def move(self, dx: float, dy: float, dtheta: float = 0.0) -> bool:
        """Move agent by given deltas.
        
        Args:
            dx: X displacement.
            dy: Y displacement.
            dtheta: Angular displacement.
            
        Returns:
            bool: True if move successful.
        """
        self._x += dx
        self._y += dy
        self._orientation += dtheta
        self._velocityX = dx
        self._velocityY = dy
        self._angularVelocity = dtheta
        self._isMoving = (abs(dx) > 0 or abs(dy) > 0 or abs(dtheta) > 0)
        return True
    
    def reset(self) -> bool:
        """Reset agent to initial state.
        
        Returns:
            bool: True if reset successful.
        """
        self._x = 0.0
        self._y = 0.0
        self._orientation = 0.0
        self._velocityX = 0.0
        self._velocityY = 0.0
        self._angularVelocity = 0.0
        self._energy = 100.0
        self._health = 100.0
        self._age = 0
        self._isMoving = False
        self._isTurning = False
        self._lastActionTime = 0.0
        return True
    
    def __repr__(self) -> str:
        return f"<AgentBody: x={self._x:.1f}, y={self._y:.1f}, orientation={self._orientation:.1f}, energy={self._energy:.1f}, health={self._health:.1f}>"


class SimpleWorld:
    """Simple 2D world for NLM simulation.
    
    This class provides a simple 2D simulation world where NLM agents
    can interact with the environment. It includes vision, object
    management, and agent state tracking.
    
    Key features:
    - 2D grid-based world
    - Vision sensing with configurable field of view
    - Object management (add, remove, query)
    - Agent body tracking
    - World time management
    
    Example:
        >>> from pynlm import createSimpleWorld
        >>> world = createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        >>> 
        >>> world.reset()
        >>> 
        >>> # Add objects
        >>> food = WorldObject(x=10, y=10, obj_type="Resource", value=50)
        >>> world.addObject(food)
        >>> 
        >>> # Check agent position
        >>> agent_body = world.getAgentBody()
        >>> print(f"Agent at ({agent_body.getX():.1f}, {agent_body.getY():.1f})")
    """
    
    def __init__(self):
        self._width = 0
        self._height = 0
        self._visionWidth = 0
        self._visionHeight = 0
        self._maxEnergy = 100.0
        self._energyDecayRate = 0.01
        self._simulationTime = 0.0
        self._agentBody = AgentBody()
        self._worldObjects: List[WorldObject] = []
        self._sensoryPercept = None
        self._randomSeed = 42
    
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> bool:
        """Configure world dimensions and vision.
        
        Args:
            width: World width in cells.
            height: World height in cells.
            visionWidth: Vision field width in cells.
            visionHeight: Vision field height in cells.
            
        Returns:
            bool: True if configured successfully.
        """
        self._width = width
        self._height = height
        self._visionWidth = visionWidth
        self._visionHeight = visionHeight
        return True
    
    def setAgentStart(self, x: float, y: float) -> bool:
        """Set agent starting position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            bool: True if position set successfully.
        """
        self._agentBody.setX(x)
        self._agentBody.setY(y)
        return True
    
    def reset(self) -> bool:
        """Reset world to initial state.
        
        Returns:
            bool: True if reset successfully.
        """
        self._agentBody.reset()
        self._worldObjects.clear()
        self._sensoryPercept = None
        self._simulationTime = 0.0
        return True
    
    def update(self, dt: float) -> bool:
        """Update world by given timestep.
        
        Args:
            dt: Time step duration.
            
        Returns:
            bool: True if update successful.
        """
        self._simulationTime += dt
        self._agentBody.setAge(int(self._simulationTime / dt))
        
        # Decay agent energy
        currentEnergy = self._agentBody.getEnergy()
        self._agentBody.setEnergy(currentEnergy * (1.0 - self._energyDecayRate * dt))
        
        # Update agent position based on velocity
        agent = self._agentBody
        newX = agent.getX() + agent.getVelocityX() * dt
        newY = agent.getY() + agent.getVelocityY() * dt
        agent.setX(newX)
        agent.setY(newY)
        
        # Keep agent in bounds
        newX = max(0.0, min(self._width - 1, newX))
        newY = max(0.0, min(self._height - 1, newY))
        agent.setX(newX)
        agent.setY(newY)
        
        return True
    
    def applyMotorCommand(self, command: Dict[str, Any], currentTime: float) -> Dict[str, Any]:
        """Apply motor command to world.
        
        Args:
            command: Motor command with type and parameters.
            currentTime: Current simulation time.
            
        Returns:
            Dict[str, Any]: Action result with reward, success, message.
        """
        action_result = ActionResult()
        
        if "type" not in command:
            action_result.setMessage("Invalid command: missing type")
            action_result.setSuccess(False)
            return action_result.__dict__
        
        action_type = command["type"]
        
        # Simple movement commands
        if action_type == "MoveForward":
            distance = command.get("distance", 1.0)
            angle = self._agentBody.getOrientation()
            dx = distance * cos(angle)
            dy = distance * sin(angle)
            self._agentBody.move(dx, dy)
            action_result.setReward(0.0)
            action_result.setSuccess(True)
            action_result.setMessage(f"Moved forward {distance:.1f} units")
            
        elif action_type == "MoveBackward":
            distance = command.get("distance", 1.0)
            angle = self._agentBody.getOrientation()
            dx = -distance * cos(angle)
            dy = -distance * sin(angle)
            self._agentBody.move(dx, dy)
            action_result.setReward(0.0)
            action_result.setSuccess(True)
            action_result.setMessage(f"Moved backward {distance:.1f} units")
            
        elif action_type == "TurnLeft":
            angle = command.get("angle", 0.1)
            self._agentBody.setOrientation(self._agentBody.getOrientation() + angle)
            action_result.setReward(0.0)
            action_result.setSuccess(True)
            action_result.setMessage(f"Turned left {angle:.1f} radians")
            
        elif action_type == "TurnRight":
            angle = command.get("angle", 0.1)
            self._agentBody.setOrientation(self._agentBody.getOrientation() - angle)
            action_result.setReward(0.0)
            action_result.setSuccess(True)
            action_result.setMessage(f"Turned right {angle:.1f} radians")
            
        elif action_type == "Look":
            # Look action - just record that it happened
            action_result.setReward(0.0)
            action_result.setSuccess(True)
            action_result.setMessage("Looking around")
            
        elif action_type == "Interact":
            # Check for objects nearby
            nearby_objects = self.getObjectsNear(self._agentBody.getX(), 
                                                self._agentBody.getY(), 
                                                1.0)
            if nearby_objects:
                obj = nearby_objects[0]
                if obj.getType() == "Resource":
                    value = obj.getValue()
                    self._agentBody.setEnergy(self._agentBody.getEnergy() + value * 0.1)
                    action_result.setReward(value * 0.1)
                    action_result.setSuccess(True)
                    action_result.setMessage(f"Collected resource worth {value:.1f}")
                else:
                    action_result.setReward(-10.0)
                    action_result.setSuccess(False)
                    action_result.setMessage("Tried to interact with hazardous object")
            else:
                action_result.setReward(-5.0)
                action_result.setSuccess(False)
                action_result.setMessage("Nothing to interact with")
        
        else:
            action_result.setReward(-1.0)
            action_result.setSuccess(False)
            action_result.setMessage(f"Unknown action type: {action_type}")
        
        return action_result.__dict__
    
    def getSensoryPercept(self) -> Dict[str, Any]:
        """Get sensory percept from world.
        
        Returns:
            Dict[str, Any]: Sensory percept data.
        """
        if self._sensoryPercept is None:
            # Create default percept if none exists
            percept = {}
            percept["vision"] = self.getVisionAt(self._agentBody.getX(), self._agentBody.getY())
            percept["time"] = self._simulationTime
            self._sensoryPercept = percept
        
        return self._sensoryPercept
    
    def getVisionAt(self, x: float, y: float) -> List[float]:
        """Get vision data at given position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            List[float]: Vision data (flattened array).
        """
        if self._width <= 0 or self._height <= 0:
            return []
        
        # Convert world coordinates to vision indices
        vision_x = int((x - self._visionWidth / 2) % self._width)
        vision_y = int((y - self._visionHeight / 2) % self._height)
        
        # Sample vision data around agent position
        vision_data = []
        for dy in range(-self._visionHeight // 2, self._visionHeight // 2):
            for dx in range(-self._visionWidth // 2, self._visionWidth // 2):
                world_x = int((vision_x + dx) % self._width)
                world_y = int((vision_y + dy) % self._height)
                
                # Find object at this location
                obj = self.getObjectAt(world_x, world_y)
                if obj:
                    if obj.getType() == "Resource":
                        vision_data.append(obj.getValue() / 100.0)  # Normalize
                    elif obj.getType() == "Hazard":
                        vision_data.append(-obj.getValue() / 100.0)  # Negative value
                    else:
                        vision_data.append(0.0)
                else:
                    vision_data.append(0.0)
        
        return vision_data
    
    def addObject(self, obj: WorldObject) -> bool:
        """Add object to world.
        
        Args:
            obj: World object to add.
            
        Returns:
            bool: True if object added successfully.
        """
        # Check if position is valid
        if not self.isValidPosition(obj.getX(), obj.getY()):
            return False
        
        self._worldObjects.append(obj)
        return True
    
    def removeObject(self, x: float, y: float) -> bool:
        """Remove object from world at given position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            bool: True if object removed successfully.
        """
        for i, obj in enumerate(self._worldObjects):
            if abs(obj.getX() - x) < 0.1 and abs(obj.getY() - y) < 0.1:
                self._worldObjects.pop(i)
                return True
        return False
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid in world.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            bool: True if position is valid.
        """
        return 0.0 <= x < self._width and 0.0 <= y < self._height
    
    def getObjectAt(self, x: float, y: float) -> Optional[WorldObject]:
        """Get object at given position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            WorldObject or None: Object at position, or None if no object.
        """
        for obj in self._worldObjects:
            if abs(obj.getX() - x) < 0.1 and abs(obj.getY() - y) < 0.1:
                return obj
        return None
    
    def getObjectsNear(self, x: float, y: float, radius: float) -> List[WorldObject]:
        """Get objects within given radius.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            radius: Search radius.
            
        Returns:
            List[WorldObject]: Objects within radius.
        """
        nearby = []
        for obj in self._worldObjects:
            distance = ((obj.getX() - x) ** 2 + (obj.getY() - y) ** 2) ** 0.5
            if distance <= radius:
                nearby.append(obj)
        return nearby
    
    def getWidth(self) -> int:
        """Get world width.
        
        Returns:
            int: World width.
        """
        return self._width
    
    def getHeight(self) -> int:
        """Get world height.
        
        Returns:
            int: World height.
        """
        return self._height
    
    def getVisionWidth(self) -> int:
        """Get vision field width.
        
        Returns:
            int: Vision field width.
        """
        return self._visionWidth
    
    def getVisionHeight(self) -> int:
        """Get vision field height.
        
        Returns:
            int: Vision field height.
        """
        return self._visionHeight
    
    def getMaxEnergy(self) -> float:
        """Get maximum agent energy.
        
        Returns:
            float: Maximum energy.
        """
        return self._maxEnergy
    
    def setMaxEnergy(self, energy: float) -> bool:
        """Set maximum agent energy.
        
        Args:
            energy: New maximum energy.
            
        Returns:
            bool: True if energy set successfully.
        """
        self._maxEnergy = energy
        return True
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            float: Energy decay rate.
        """
        return self._energyDecayRate
    
    def setEnergyDecayRate(self, rate: float) -> bool:
        """Set energy decay rate.
        
        Args:
            rate: New energy decay rate.
            
        Returns:
            bool: True if rate set successfully.
        """
        self._energyDecayRate = rate
        return True
    
    def getAgentBody(self) -> AgentBody:
        """Get agent body.
        
        Returns:
            AgentBody: Agent body object.
        """
        return self._agentBody
    
    def getSimulationTime(self) -> float:
        """Get simulation time.
        
        Returns:
            float: Simulation time.
        """
        return self._simulationTime
    
    def setRandomSeed(self, seed: int) -> bool:
        """Set random seed.
        
        Args:
            seed: Random seed.
            
        Returns:
            bool: True if seed set successfully.
        """
        self._randomSeed = seed
        return True
    
    def getRandomSeed(self) -> int:
        """Get random seed.
        
        Returns:
            int: Random seed.
        """
        return self._randomSeed
    
    def __repr__(self) -> str:
        return f"<SimpleWorld: {self._width}x{self._height}, vision={self._visionWidth}x{self._visionHeight}, objects={len(self._worldObjects)}>"
