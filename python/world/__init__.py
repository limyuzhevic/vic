"""
World module for NLM

This module provides the SimpleWorld class for simulation environments.
"""

import math
from typing import List, Optional
from .agent import AgentBody
from .sensory import SensoryPercept
from .motor import Action, ActionType
from .core import LogLevel, WorldObject, WorldObjectType
from .brain import Brain

class SimpleWorld:
    def __init__(self):
        self._width = 16
        self._height = 16
        self._vision_width = 8
        self._vision_height = 8
        self._agent_x = 8.0
        self._agent_y = 8.0
        self._agent_orientation = 0.0
        self._simulation_time = 0.0
        self._max_energy = 100.0
        self._energy_decay_rate = 0.01
        self._objects: List[WorldObject] = []
        self._random_seed = 42

    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions"""
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height

    def reset(self):
        """Reset world state"""
        self._agent_x = self._width / 2.0
        self._agent_y = self._height / 2.0
        self._agent_orientation = 0.0
        self._simulation_time = 0.0
        self._objects.clear()
        self._setRandomSeed(self._random_seed)

    def setAgentStart(self, x: float, y: float):
        """Set agent starting position"""
        self._agent_x = x
        self._agent_y = y

    def update(self, timestep: float):
        """Update world state"""
        try:
            self._simulation_time += timestep
            self._energy_decay_rate = max(0.0, self._energy_decay_rate - timestep * 0.001)
        except Exception:
            pass

    def applyMotorCommand(self, cmd: Action, current_time: float):
        """Apply motor command to world"""
        try:
            if cmd.getType() == ActionType.MoveForward:
                self._agent_x += math.cos(self._agent_orientation)
                self._agent_y += math.sin(self._agent_orientation)
            elif cmd.getType() == ActionType.TurnLeft:
                self._agent_orientation -= math.pi / 4.0
            elif cmd.getType() == ActionType.TurnRight:
                self._agent_orientation += math.pi / 4.0
            
            # Clamp to world bounds
            self._agent_x = max(0.0, min(self._width, self._agent_x))
            self._agent_y = max(0.0, min(self._height, self._agent_y))
        except Exception:
            pass

    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept from world"""
        percept = SensoryPercept()
        
        # Get vision data
        from .sensory import Vision
        vision = Vision(self._vision_width, self._vision_height)
        
        # Simple vision simulation - fill with gradient based on agent position
        for y in range(self._vision_height):
            for x in range(self._vision_width):
                # Calculate distance from agent
                world_x = self._agent_x + x - self._vision_width / 2.0
                world_y = self._agent_y + y - self._vision_height / 2.0
                distance = math.sqrt((world_x - self._width/2)**2 + (world_y - self._height/2)**2)
                normalized_distance = min(1.0, distance / max(self._width, self._height))
                
                # Create vision data
                vision_data = []
                for c in range(3):  # RGB channels
                    val = int(255 * (1.0 - normalized_distance))
                    vision_data.append(float(val))
                
                vision.setData(vision_data)
        
        percept.setVision(vision)
        return percept

    def getAgentBody(self) -> AgentBody:
        """Get agent body state"""
        body = AgentBody()
        body.x = self._agent_x
        body.y = self._agent_y
        body.orientation = self._agent_orientation
        body.velocityX = 0.0
        body.velocityY = 0.0
        body.angularVelocity = 0.0
        body.energy = self._max_energy * self._energy_decay_rate
        body.health = 100.0
        body.age = int(self._simulation_time)
        body.isMoving = False
        body.isTurning = False
        body.lastActionTime = self._simulation_time
        return body

    def addObject(self, obj: WorldObject):
        """Add object to world"""
        self._objects.append(obj)

    def removeObject(self, x: float, y: float):
        """Remove object from world"""
        self._objects = [obj for obj in self._objects if not (math.isclose(obj.x, x) and math.isclose(obj.y, y))]

    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid"""
        return (0.0 <= x <= self._width and 0.0 <= y <= self._height and
                not any(math.isclose(obj.x, x) and math.isclose(obj.y, y) for obj in self._objects))

    def getWidth(self) -> int:
        return self._width

    def getHeight(self) -> int:
        return self._height

    def getMaxEnergy(self) -> float:
        return self._max_energy

    def setMaxEnergy(self, e: float):
        self._max_energy = e

    def getEnergyDecayRate(self) -> float:
        return self._energy_decay_rate

    def setEnergyDecayRate(self, r: float):
        self._energy_decay_rate = r

    def getSimulationTime(self) -> float:
        return self._simulation_time

    def setRandomSeed(self, seed: int):
        """Set random seed for reproducible simulations"""
        self._random_seed = seed

    def getRandomSeed(self) -> int:
        return self._random_seed

    def _setRandomSeed(self, seed: int):
        """Internal method to set random seed"""
        import random
        random.seed(seed)

    def computeReward(self, brain: Brain) -> float:
        """Compute reward based on agent state and brain"""
        # Simple reward based on movement and activity
        reward = 0.0
        
        # Reward for staying in center
        center_x, center_y = self._width / 2.0, self._height / 2.0
        distance_from_center = math.sqrt((self._agent_x - center_x)**2 + (self._agent_y - center_y)**2)
        reward += max(0.0, 1.0 - distance_from_center / max(self._width, self._height))
        
        # Reward for exploration
        reward += brain.getTotalSpikeCount() * 0.001
        
        # Penalty for low energy
        energy_ratio = self._energy_decay_rate / 1.0
        reward -= (1.0 - energy_ratio) * 0.1
        
        return reward

    def applyAction(self, brain: Brain, cmd: Action):
        """Apply action to world (simplified)"""
        self.applyMotorCommand(cmd, self._simulation_time)

    def __repr__(self):
        return f"SimpleWorld(width={self._width}, height={self._height}, agent=({self._agent_x:.1f}, {self._agent_y:.1f}))"

# Convenience function

def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world"""
    return SimpleWorld()

__all__ = [
    'SimpleWorld',
    'createSimpleWorld',
]