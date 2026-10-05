"""
AgentBody class for NLM neural simulation framework.

This module provides a Python interface to the C++ AgentBody class with enhanced
Python features including automatic numpy conversion, error formatting, and
time measurement utilities.
"""

import time
import numpy as np
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
import traceback

# Python utilities
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
)

@dataclass
class AgentBodyState:
    """State information for agent body"""
    x: float = 0.0
    y: float = 0.0
    orientation: float = 0.0
    velocity_x: float = 0.0
    velocity_y: float = 0.0
    angular_velocity: float = 0.0
    energy: float = 100.0
    health: float = 100.0
    age: float = 0.0
    is_moving: bool = False
    is_turning: bool = False
    last_action_time: float = 0.0

class AgentBody:
    """
    Agent body state class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ AgentBody class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self):
        """Initialize agent body state."""
        self._logger = PythonSessionLogger()
        self._state = AgentBodyState()
        
        # Log initialization
        self._logger.logEntry("AgentBody initialized")
    
    # Position and movement properties
    
    @property
    def x(self) -> float:
        """Get x-coordinate."""
        return self._state.x
    
    @x.setter
    def x(self, value: float) -> None:
        """Set x-coordinate."""
        self._state.x = value
        self._state.is_moving = True
    
    @property
    def y(self) -> float:
        """Get y-coordinate."""
        return self._state.y
    
    @y.setter
    def y(self, value: float) -> None:
        """Set y-coordinate."""
        self._state.y = value
        self._state.is_moving = True
    
    @property
    def orientation(self) -> float:
        """Get orientation (in degrees)."""
        return self._state.orientation
    
    @orientation.setter
    def orientation(self, value: float) -> None:
        """Set orientation (in degrees)."""
        self._state.orientation = value
        self._state.is_turning = True
    
    @property
    def velocity_x(self) -> float:
        """Get x-velocity."""
        return self._state.velocity_x
    
    @velocity_x.setter
    def velocity_x(self, value: float) -> None:
        """Set x-velocity."""
        self._state.velocity_x = value
        self._state.is_moving = True
    
    @property
    def velocity_y(self) -> float:
        """Get y-velocity."""
        return self._state.velocity_y
    
    @velocity_y.setter
    def velocity_y(self, value: float) -> None:
        """Set y-velocity."""
        self._state.velocity_y = value
        self._state.is_moving = True
    
    @property
    def angular_velocity(self) -> float:
        """Get angular velocity."""
        return self._state.angular_velocity
    
    @angular_velocity.setter
    def angular_velocity(self, value: float) -> None:
        """Set angular velocity."""
        self._state.angular_velocity = value
        self._state.is_turning = True
    
    @property
    def energy(self) -> float:
        """Get energy level."""
        return self._state.energy
    
    @energy.setter
    def energy(self, value: float) -> None:
        """Set energy level."""
        self._state.energy = max(0.0, min(100.0, value))
    
    @property
    def health(self) -> float:
        """Get health level."""
        return self._state.health
    
    @health.setter
    def health(self, value: float) -> None:
        """Set health level."""
        self._state.health = max(0.0, min(100.0, value))
    
    @property
    def age(self) -> float:
        """Get age."""
        return self._state.age
    
    @age.setter
    def age(self, value: float) -> None:
        """Set age."""
        self._state.age = max(0.0, value)
    
    @property
    def is_moving(self) -> bool:
        """Check if agent is moving."""
        return self._state.is_moving
    
    @property
    def is_turning(self) -> bool:
        """Check if agent is turning."""
        return self._state.is_turning
    
    @property
    def last_action_time(self) -> float:
        """Get last action time."""
        return self._state.last_action_time
    
    @last_action_time.setter
    def last_action_time(self, value: float) -> None:
        """Set last action time."""
        self._state.last_action_time = value
    
    # Movement methods
    
    def move_forward(self, distance: float = 1.0) -> None:
        """
        Move forward by specified distance.
        
        Args:
            distance: Distance to move (default: 1.0)
        """
        # Calculate new position
        angle_rad = np.radians(self._state.orientation)
        self.x += distance * np.cos(angle_rad)
        self.y += distance * np.sin(angle_rad)
        
        # Update velocity
        self.velocity_x = np.cos(angle_rad)
        self.velocity_y = np.sin(angle_rad)
        
        # Update energy (consumption)
        self.energy -= distance * 0.1
        
        # Update action time
        self.last_action_time = time.time()
        
        self._logger.logEntry(f"Moved forward: distance={distance}, position=({self.x:.2f}, {self.y:.2f})")
    
    def move_backward(self, distance: float = 1.0) -> None:
        """
        Move backward by specified distance.
        
        Args:
            distance: Distance to move (default: 1.0)
        """
        # Calculate new position
        angle_rad = np.radians(self._state.orientation)
        self.x -= distance * np.cos(angle_rad)
        self.y -= distance * np.sin(angle_rad)
        
        # Update velocity
        self.velocity_x = -np.cos(angle_rad)
        self.velocity_y = -np.sin(angle_rad)
        
        # Update energy (consumption)
        self.energy -= distance * 0.1
        
        # Update action time
        self.last_action_time = time.time()
        
        self._logger.logEntry(f"Moved backward: distance={distance}, position=({self.x:.2f}, {self.y:.2f})")
    
    def turn_left(self, angle: float = 90.0) -> None:
        """
        Turn left by specified angle.
        
        Args:
            angle: Angle to turn in degrees (default: 90.0)
        """
        self.orientation += angle
        self.angular_velocity = angle / 0.1  # Assuming 0.1 second time step
        
        # Update energy (consumption)
        self.energy -= angle * 0.01
        
        # Update action time
        self.last_action_time = time.time()
        
        self._logger.logEntry(f"Turned left: angle={angle}, orientation={self.orientation}")
    
    def turn_right(self, angle: float = 90.0) -> None:
        """
        Turn right by specified angle.
        
        Args:
            angle: Angle to turn in degrees (default: 90.0)
        """
        self.orientation -= angle
        self.angular_velocity = -angle / 0.1  # Assuming 0.1 second time step
        
        # Update energy (consumption)
        self.energy -= angle * 0.01
        
        # Update action time
        self.last_action_time = time.time()
        
        self._logger.logEntry(f"Turned right: angle={angle}, orientation={self.orientation}")
    
    def look(self, direction: str = "forward") -> None:
        """
        Look in specified direction.
        
        Args:
            direction: Direction to look (default: "forward")
        """
        # Update action time
        self.last_action_time = time.time()
        
        # In real implementation, this would affect agent's perception
        self._logger.logEntry(f"Looking: direction={direction}")
    
    def look_up(self) -> None:
        """Look up."""
        self.last_action_time = time.time()
        self._logger.logEntry("Looking up")
    
    def look_down(self) -> None:
        """Look down."""
        self.last_action_time = time.time()
        self._logger.logEntry("Looking down")
    
    def interact(self) -> None:
        """Interact with environment."""
        self.last_action_time = time.time()
        self.energy -= 5.0  # Energy cost for interaction
        self._logger.logEntry("Interacting with environment")
    
    def eat(self) -> None:
        """Eat food."""
        self.last_action_time = time.time()
        self.energy += 10.0  # Energy gain from eating
        self.health += 5.0   # Health gain from eating
        self._logger.logEntry("Eating food")
    
    def drink(self) -> None:
        """Drink water."""
        self.last_action_time = time.time()
        self.health += 3.0   # Health gain from drinking
        self._logger.logEntry("Drinking water")
    
    def rest(self) -> None:
        """Rest and recover."""
        self.last_action_time = time.time()
        self.energy += 20.0  # Energy recovery
        self.health += 2.0   # Health recovery
        self._logger.logEntry("Resting and recovering")
    
    def wait(self, duration: float = 1.0) -> None:
        """
        Wait for specified duration.
        
        Args:
            duration: Wait duration in seconds (default: 1.0)
        """
        time.sleep(duration)
        self.last_action_time = time.time()
        self._logger.logEntry(f"Waiting: duration={duration}")
    
    def reset(self) -> None:
        """Reset agent body state."""
        self._state = AgentBodyState()
        self._logger.logEntry("AgentBody reset")
    
    def __repr__(self) -> str:
        """String representation of agent body."""
        return f"<AgentBody: ({self.x:.2f}, {self.y:.2f}), orientation={self.orientation:.1f}>"
    
    def __str__(self) -> str:
        """String representation of agent body."""
        return f"NLM Agent Body at ({self.x:.2f}, {self.y:.2f}), orientation={self.orientation:.1f}"

__all__ = ['AgentBody']
