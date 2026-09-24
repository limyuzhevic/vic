# World simulation and environment management for NLM

"""
World simulation and environment management for NLM (Neural Learning Machine).

This module provides classes for simulating environments, managing world objects,
and handling sensory input and motor output interactions.
"""

import numpy as np
import time
from typing import List, Dict, Optional, Any, Tuple
from enum import Enum

from ._cpp import (
    WorldObject, WorldObjectType, SimpleWorld,
    SensoryInput, Vision, Audio, InternalSignals,
    Action, ActionResult, SensoryPercept,
    ActionType, MotorCommand
)

__all__ = [
    'WorldObject', 'WorldObjectType', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'ActionResult', 'SensoryPercept',
    'ActionType', 'MotorCommand',
    'createSimpleWorld', 'createWorldObject', 'createObservation',
    'WorldManager', 'EnvironmentSimulator',
    'GridWorld', 'ContinuousWorld', 'SimulationTime',
]


class WorldManager:
    """
    Manager for world simulation and environment interactions.

    This class provides a high-level interface for managing worlds,
    handling object interactions, and coordinating sensory-motor loops.
    """

    def __init__(self, width: int = 20, height: int = 20):
        """
        Initialize world manager.

        Args:
            width: World width in units.
            height: World height in units.
        """
        self.width = width
        self.height = height
        self.world = createSimpleWorld()
        self.world.configure(width, height, 8, 8)  # Default vision size
        self.world.reset()
        self.objects: List[WorldObject] = []
        self.agent_start = (width // 2, height // 2)
        self.world.setAgentStart(self.agent_start[0], self.agent_start[1])

    def addObject(self, x: float, y: float, obj_type: WorldObjectType,
                  value: float = 0.0, radius: float = 0.5) -> WorldObject:
        """
        Add an object to the world.

        Args:
            x: X coordinate.
            y: Y coordinate.
            obj_type: Type of object.
            value: Object value (for resources/hazards).
            radius: Object radius.

        Returns:
            Created WorldObject.
        """
        obj = WorldObject(x, y, obj_type, value, radius)
        self.objects.append(obj)
        self.world.addObject(obj)
        return obj

    def removeObject(self, x: float, y: float):
        """
        Remove object at specified position.

        Args:
            x: X coordinate.
            y: Y coordinate.
        """
        self.world.removeObject(x, y)
        # Remove from objects list
        self.objects = [obj for obj in self.objects if not (abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]

    def update(self, timestep: float):
        """
        Update world state.

        Args:
            timestep: Time step size.
        """
        self.world.update(timestep)

    def getSensoryPercept(self) -> SensoryPercept:
        """
        Get current sensory perception.

        Returns:
            Current sensory perception.
        """
        return self.world.getSensoryPercept()

    def getAgentBody(self) -> Any:
        """
        Get current agent body state.

        Args:
            Returns agent body state.
        """
        return self.world.getAgentBody()

    def applyAction(self, action: Action, current_time: float) -> ActionResult:
        """
        Apply action to the world.

        Args:
            action: Action to apply.
            current_time: Current simulation time.

        Returns:
            Action result.
        """
        return self.world.applyMotorCommand(action, current_time)

    def isValidPosition(self, x: float, y: float) -> bool:
        """
        Check if position is valid.

        Args:
            x: X coordinate.
            y: Y coordinate.

        Returns:
            True if position is valid, False otherwise.
        """
        return self.world.isValidPosition(x, y)

    def getWidth(self) -> int:
        """Get world width."""
        return self.world.getWidth()

    def getHeight(self) -> int:
        """Get world height."""
        return self.world.getHeight()

    def reset(self):
        """Reset world to initial state."""
        self.world.reset()
        self.objects.clear()
        self.world.setAgentStart(self.agent_start[0], self.agent_start[1])


class EnvironmentSimulator:
    """
    High-level environment simulator with event handling.

    This class provides advanced environment simulation capabilities
    including event-driven updates, reward calculation, and agent interaction.
    """

    def __init__(self, world_manager: WorldManager):
        """
        Initialize environment simulator.

        Args:
            world_manager: World manager instance.
        """
        self.world_manager = world_manager
        self.simulation_time = 0.0
        self.events: List[Dict] = []
        self.reward_history: List[float] = []
        self.observation_history: List[SensoryPercept] = []

    def step(self, action: Action, timestep: float = 0.1) -> Tuple[ActionResult, SensoryPercept]:
        """
        Take a step in the environment.

        Args:
            action: Action to take.
            timestep: Time step size.

        Returns:
            Tuple of (action result, new observation).
        """
        # Update world time
        self.simulation_time += timestep

        # Apply action
        result = self.world_manager.applyAction(action, self.simulation_time)

        # Update world
        self.world_manager.update(timestep)

        # Get new observation
        observation = self.world_manager.getSensoryPercept()

        # Record history
        self.reward_history.append(result.reward)
        self.observation_history.append(observation)

        # Create environment event
        event = {
            'time': self.simulation_time,
            'action': action,
            'result': result,
            'observation': observation,
            'reward': result.reward
        }
        self.events.append(event)

        return result, observation

    def reset(self):
        """Reset environment to initial state."""
        self.world_manager.reset()
        self.simulation_time = 0.0
        self.events.clear()
        self.reward_history.clear()
        self.observation_history.clear()

    def getTotalReward(self) -> float:
        """Get total accumulated reward."""
        return sum(self.reward_history)

    def getAverageReward(self) -> float:
        """Get average reward."""
        if not self.reward_history:
            return 0.0
        return sum(self.reward_history) / len(self.reward_history)

    def getSimulationTime(self) -> float:
        """Get current simulation time."""
        return self.simulation_time

    def getEventHistory(self) -> List[Dict]:
        """Get event history."""
        return self.events.copy()

    def getRewardHistory(self) -> List[float]:
        """Get reward history."""
        return self.reward_history.copy()


class GridWorld(WorldManager):
    """
    Specialized grid world for NLM simulation.

    This class provides a grid-based world with predefined objects
    and terrain features.
    """

    def __init__(self, width: int = 20, height: int = 20):
        """
        Initialize grid world.

        Args:
            width: World width.
            height: World height.
        """
        super().__init__(width, height)
        self.terrain = np.zeros((width, height))
        self.initializeTerrain()

    def initializeTerrain(self):
        """Initialize world terrain with obstacles and features."""
        # Clear terrain
        self.terrain.fill(0.0)

        # Add walls
        for i in range(self.width):
            self.terrain[i][0] = 1.0  # Left wall
            self.terrain[i][self.height - 1] = 1.0  # Right wall
        for j in range(self.height):
            self.terrain[0][j] = 1.0  # Top wall
            self.terrain[self.width - 1][j] = 1.0  # Bottom wall

        # Add random obstacles
        import random
        for _ in range(min(self.width * self.height // 20, 50)):
            x = random.randint(1, self.width - 2)
            y = random.randint(1, self.height - 2)
            if self.terrain[x][y] == 0:
                self.terrain[x][y] = 0.5  # Obstacle

        # Add resources
        for _ in range(min(self.width * self.height // 50, 20)):
            x = random.randint(1, self.width - 2)
            y = random.randint(1, self.height - 2)
            if self.terrain[x][y] == 0:
                obj_type = WorldObjectType.Resource if random.random() > 0.5 else WorldObjectType.Hazard
                self.addObject(float(x), float(y), obj_type, 1.0 if obj_type == WorldObjectType.Resource else -1.0)

    def setAgentStart(self, x: float, y: float):
        """Set agent starting position."""
        super().setAgentStart(x, y)

    def getTerrain(self) -> np.ndarray:
        """Get terrain map."""
        return self.terrain.copy()


class ContinuousWorld(WorldManager):
    """
    Continuous world for smooth movement and physics.

    This class provides a continuous world with physics simulation
    and smooth movement capabilities.
    """

    def __init__(self, width: float = 100.0, height: float = 100.0):
        """
        Initialize continuous world.

        Args:
            width: World width in continuous units.
            height: World height in continuous units.
        """
        super().__init__(int(width), int(height))
        self.continuous_width = width
        self.continuous_height = height
        self.agent_velocity = [0.0, 0.0]
        self.agent_acceleration = [0.0, 0.0]
        self.friction = 0.95

    def applyPhysics(self, force_x: float = 0.0, force_y: float = 0.0, timestep: float = 0.1):
        """
        Apply physics forces to agent.

        Args:
            force_x: X force.
            force_y: Y force.
            timestep: Time step.
        """
        # Update velocity with force
        self.agent_acceleration[0] = force_x / 1.0  # Mass = 1.0
        self.agent_acceleration[1] = force_y / 1.0

        # Update velocity
        self.agent_velocity[0] += self.agent_acceleration[0] * timestep
        self.agent_velocity[1] += self.agent_acceleration[1] * timestep

        # Apply friction
        self.agent_velocity[0] *= self.friction
        self.agent_velocity[1] *= self.friction

        # Update agent position
        body = self.getAgentBody()
        new_x = body.x + self.agent_velocity[0] * timestep
        new_y = body.y + self.agent_velocity[1] * timestep

        # Check bounds
        new_x = max(1.0, min(self.continuous_width - 1.0, new_x))
        new_y = max(1.0, min(self.continuous_height - 1.0, new_y))

        # Update body
        body.x = new_x
        body.y = new_y

    def reset(self):
        """Reset world and physics state."""
        super().reset()
        self.agent_velocity = [0.0, 0.0]
        self.agent_acceleration = [0.0, 0.0]


class SimulationTime:
    """
    Utility class for managing simulation time.
    """

    @staticmethod
    def format_time(seconds: float) -> str:
        """
        Format time in seconds to human-readable format.

        Args:
            seconds: Time in seconds.

        Returns:
            Formatted time string.
        """
        if seconds < 60:
            return f"{seconds:.2f}s"
        elif seconds < 3600:
            minutes = seconds / 60
            return f"{minutes:.1f}m"
        else:
            hours = seconds / 3600
            return f"{hours:.1f}h"

    @staticmethod
    def calculateSteps(duration: float, timestep: float) -> int:
        """
        Calculate number of steps for given duration.

        Args:
            duration: Simulation duration in seconds.
            timestep: Time step size.

        Returns:
            Number of steps.
        """
        return int(duration / timestep)

    @staticmethod
    def validateTimestep(timestep: float) -> bool:
        """
        Validate time step size.

        Args:
            timestep: Time step to validate.

        Returns:
            True if timestep is valid, False otherwise.
        """
        return timestep > 0.0 and timestep < 1.0


# Convenience functions

def createSimpleWorld() -> SimpleWorld:
    """Create a simple world for simulation."""
    return SimpleWorld()


def createWorldObject(x: float, y: float, obj_type: WorldObjectType,
                      value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create a world object."""
    return WorldObject(x, y, obj_type, value, radius)


def createObservation() -> SensoryPercept:
    """Create a sensory observation."""
    return SensoryPercept()


# Export commonly used constants and functions
__all__.extend([
    'GridWorld', 'ContinuousWorld', 'SimulationTime',
    'WorldManager', 'EnvironmentSimulator',
    'createSimpleWorld', 'createWorldObject', 'createObservation'
])