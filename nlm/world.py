"""
World system classes and interfaces
"""

from typing import List, Optional, Dict, Any
from enum import Enum

from .types import WorldObjectType

class AgentBody:
    """Agent body state"""
    
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def reset(self):
        """Reset agent body"""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
class WorldObject:
    """World object representation"""
    
    def __init__(self, x: float = 0.0, y: float = 0.0,
                 obj_type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        self.x = x
        self.y = y
        self.obj_type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            "x": self.x,
            "y": self.y,
            "type": self.obj_type,
            "type_name": self.obj_type.name if self.obj_type else "",
            "value": self.value,
            "radius": self.radius,
            "active": self.active,
        }
class SimpleWorld:
    """Simple 2D world for NLM simulation"""
    
    def __init__(self):
        self.width = 800
        self.height = 600
        self.vision_width = 80
        self.vision_height = 80
        self.agent = AgentBody()
        self.objects: List[WorldObject] = []
        self.simulation_time = 0.0
        self.random_seed = 42
    
    def configure(self, width: int, height: int,
                  vision_width: int, vision_height: int):
        """Configure world dimensions"""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
    
    def reset(self):
        """Reset world state"""
        self.agent.reset()
        self.objects.clear()
        self.simulation_time = 0.0
    
    def setAgentStart(self, x: float, y: float):
        """Set agent starting position"""
        self.agent.x = x
        self.agent.y = y
        self.agent.isMoving = False
        self.agent.isTurning = False
    
    def update(self, timestep: float):
        """Update world simulation"""
        self.simulation_time += timestep
        self.agent.age += 1
        
        # Update agent physics
        if self.agent.isMoving:
            self.agent.x += self.agent.velocityX * timestep
            self.agent.y += self.agent.velocityY * timestep
        
        # Check boundaries
        self.agent.x = max(0, min(self.width - self.agent.radius, self.agent.x))
        self.agent.y = max(0, min(self.height - self.agent.radius, self.agent.y))
    
    def applyMotorCommand(self, action, current_time: float):
        """Apply motor command to world"""
        # Simplified motor command application
        if action.getType() == ActionType.MoveForward:
            self.agent.isMoving = True
            self.agent.velocityX = 2.0
            self.agent.velocityY = 0.0
        elif action.getType() == ActionType.MoveBackward:
            self.agent.isMoving = True
            self.agent.velocityX = -2.0
            self.agent.velocityY = 0.0
        elif action.getType() == ActionType.TurnLeft:
            self.agent.isTurning = True
            self.agent.angularVelocity = 0.05
        elif action.getType() == ActionType.TurnRight:
            self.agent.isTurning = True
            self.agent.angularVelocity = -0.05
        else:
            self.agent.isMoving = False
            self.agent.isTurning = False
            self.agent.velocityX = 0.0
            self.agent.velocityY = 0.0
            self.agent.angularVelocity = 0.0
        
        self.agent.lastActionTime = current_time
        self.update(0.001)
    
    def getSensoryPercept(self):
        """Get sensory percept from world"""
        # Create simple percept based on agent surroundings
        from .sensory import SensoryPercept
        
        percept = SensoryPercept()
        
        # Simple vision - create dummy vision data
        from .sensory import Vision
        vision_data = []
        for i in range(self.vision_width):
            row = []
            for j in range(self.vision_height):
                # Check if there's an object nearby
                nearby_objects = [
                    obj for obj in self.objects 
                    if ((obj.x - self.agent.x) ** 2 + (obj.y - self.agent.y) ** 2) ** 0.5 < 10.0
                ]
                brightness = min(1.0, len(nearby_objects) * 0.2) if nearby_objects else 0.0
                row.append(brightness)
            vision_data.append(row)
        
        vision = Vision(self.vision_width, self.vision_height, 1)
        vision.set_normalized_data(vision_data)
        percept.setVision(vision)
        
        return percept
    
    def getAgentBody(self) -> AgentBody:
        """Get agent body"""
        return self.agent
    
    def addObject(self, obj: WorldObject):
        """Add world object"""
        self.objects.append(obj)
    
    def removeObject(self, x: float, y: float):
        """Remove object at position"""
        self.objects = [obj for obj in self.objects 
                       if not (abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid (not occupied by wall)"""
        return not any(
            abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1 and obj.obj_type == WorldObjectType.Wall
            for obj in self.objects
        )
    
    def getWidth(self) -> int:
        """Get world width"""
        return self.width
    
    def getHeight(self) -> int:
        """Get world height"""
        return self.height
    
    def getMaxEnergy(self) -> float:
        """Get max energy"""
        return 100.0
    
    def setMaxEnergy(self, e: float):
        """Set max energy"""
        pass
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate"""
        return 0.01
    
    def setEnergyDecayRate(self, r: float):
        """Set energy decay rate"""
        pass
    
    def getSimulationTime(self) -> float:
        """Get simulation time"""
        return self.simulation_time
    
    def setRandomSeed(self, seed: int):
        """Set random seed"""
        self.random_seed = seed
    
    def getRandomSeed(self) -> int:
        """Get random seed"""
        return self.random_seed
    
    def getObjects(self) -> List[WorldObject]:
        """Get all objects"""
        return self.objects
