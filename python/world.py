# NLM World Python Module

"""
World module for NLM Python interface
"""

from . import __init__ as nlm_init
from typing import List, Optional, Union, Any
from pathlib import Path

# Re-export from main module
Brain = nlm_init.Brain
Config = nlm_init.Config
createBrain = nlm_init.createBrain
createDefaultConfig = nlm_init.createDefaultConfig
ActionType = nlm_init.ActionType
MotorCommand = nlm_init.MotorCommand
WorldObjectType = nlm_init.WorldObjectType
Vision = nlm_init.Vision
Audio = nlm_init.Audio
SensoryPercept = nlm_init.SensoryPercept
WorldObject = nlm_init.WorldObject
AgentBody = nlm_init.AgentBody
ActionResult = nlm_init.ActionResult
Action = nlm_init.Action

# World object factory functions
def createWorldObject(x: float = 0.0, y: float = 0.0, 
                     obj_type: WorldObjectType = WorldObjectType.Empty,
                     value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create a world object"""
    return WorldObject(x, y, obj_type, value, radius)

# SimpleWorld interface (to be implemented)
class SimpleWorld:
    """
    Simple 2D world for NLM simulation
    
    Represents the environment in which the NLM brain operates,
    providing sensory input and handling actions.
    """
    
    def __init__(self):
        # Initialize C++ world (to be implemented)
        self._cpp_world = None
    
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None:
        """Configure world dimensions and sensory range"""
        # To be implemented
        pass
    
    def reset(self) -> None:
        """Reset world to initial state"""
        # To be implemented
        pass
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position"""
        # To be implemented
        pass
    
    def update(self, timestep: float) -> None:
        """Update world state by timestep seconds"""
        # To be implemented
        pass
    
    def applyMotorCommand(self, cmd: Any, currentTime: float) -> Any:
        """Apply motor command to world and return result"""
        # To be implemented
        pass
    
    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept from world"""
        # To be implemented
        percept = SensoryPercept()
        # TODO: Implement actual percept generation
        return percept
    
    def getAgentBody(self) -> AgentBody:
        """Get agent body state"""
        # To be implemented
        return AgentBody()
    
    def addObject(self, obj: WorldObject) -> None:
        """Add object to world"""
        # To be implemented
        pass
    
    def removeObject(self, x: float, y: float) -> None:
        """Remove object at position"""
        # To be implemented
        pass
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid in world"""
        # To be implemented
        return True
    
    def getWidth(self) -> int:
        """Get world width"""
        # To be implemented
        return 0
    
    def getHeight(self) -> int:
        """Get world height"""
        # To be implemented
        return 0
    
    def getMaxEnergy(self) -> float:
        """Get maximum agent energy"""
        # To be implemented
        return 100.0
    
    def setMaxEnergy(self, energy: float) -> None:
        """Set maximum agent energy"""
        # To be implemented
        pass
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate"""
        # To be implemented
        return 0.01
    
    def setEnergyDecayRate(self, rate: float) -> None:
        """Set energy decay rate"""
        # To be implemented
        pass
    
    def getSimulationTime(self) -> float:
        """Get current simulation time"""
        # To be implemented
        return 0.0
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed"""
        # To be implemented
        pass
    
    def getRandomSeed(self) -> int:
        """Get random seed"""
        # To be implemented
        return 42


def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world"""
    return SimpleWorld()


# World utility functions
def isInBounds(world: SimpleWorld, x: float, y: float) -> bool:
    """Check if position is within world bounds"""
    return (0 <= x < world.getWidth() and 0 <= y < world.getHeight())


def distance(x1: float, y1: float, x2: float, y2: float) -> float:
    """Calculate Euclidean distance between two points"""
    return ((x2 - x1) ** 2 + (y2 - y1) ** 2) ** 0.5


__all__ = [
    "SimpleWorld", "createSimpleWorld",
    "isInBounds", "distance"
]
