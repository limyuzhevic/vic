"""
Motor control module for NLM.

This module provides classes and functions for motor control, action generation,
and agent movement in the simulated environment.
"""

from typing import Dict, Any, Optional, List, Tuple
from enum import Enum
import math
from contextlib import contextmanager

from .exceptions import (
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
    ResourceError,
)

class ActionType(Enum):
    """Enumeration of action types."""
    MoveForward = "move_forward"
    TurnLeft = "turn_left"
    TurnRight = "turn_right"
    Interact = "interact"
    Eat = "eat"
    MoveBackward = "move_backward"
class MotorCommand:
    """
    Represents a motor command for agent movement.
    
    This class encodes motor commands with action type, magnitude, and
    duration information for agent control.
    """

    def __init__(
        self,
        action_type: ActionType,
        magnitude: float = 1.0,
        duration: Optional[float] = None,
    ):
        """
        Initialize a new motor command.
        
        Args:
            action_type: Type of action
            magnitude: Magnitude of action
            duration: Duration of action (optional)
        """
        self._action_type = action_type
        self._magnitude = magnitude
        self._duration = duration
        self._timestamp = None

    def __repr__(self) -> str:
        """String representation."""
        return f"MotorCommand(type={self._action_type}, magnitude={self._magnitude})"

    # Properties for Pythonic interface

    @property
    def action_type(self) -> ActionType:
        """Get action type."""
        return self._action_type

    @property
    def magnitude(self) -> float:
        """Get magnitude."""
        return self._magnitude

    @property
    def duration(self) -> Optional[float]:
        """Get duration."""
        return self._duration

    @property
    def timestamp(self) -> Optional[float]:
        """Get timestamp."""
        return self._timestamp

    @timestamp.setter
    def timestamp(self, value: float) -> None:
        """Set timestamp."""
        self._timestamp = value


class MotorController:
    """
    Controls agent movement and motor actions.
    
    This class provides functions for generating and executing motor commands,
    including movement control, turning, and interaction with the environment.
    """

    def __init__(self):
        """Initialize a new MotorController instance."""
        self._initialized = False
        self._config = {}
        self._last_command = None

    def __enter__(self):
        """Context manager entry point."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.reset()

    def initialize(self, config: Optional[Dict[str, Any]] = None) -> None:
        """
        Initialize the motor controller.
        
        Args:
            config: Optional configuration dictionary
        """
        self._config = config or {}
        self._initialized = True

    def generate_command(self, action_type: ActionType, magnitude: float = 1.0) -> MotorCommand:
        """
        Generate a motor command.
        
        Args:
            action_type: Type of action
            magnitude: Magnitude of action
            
        Returns:
            Motor command instance
        """
        if not self._initialized:
            raise InitializationError("MotorController not initialized. Call initialize() first.")

        command = MotorCommand(action_type, magnitude)
        self._last_command = command

        return command

    def execute_command(self, command: MotorCommand, position: Tuple[float, float]) -> Dict[str, Any]:
        """
        Execute a motor command from a position.
        
        Args:
            command: Motor command to execute
            position: Current agent position (x, y)
            
        Returns:
            Dictionary with execution results
        """
        if not self._initialized:
            raise InitializationError("MotorController not initialized. Call initialize() first.")

        x, y = position
        angle = 0.0  # Simplified: assume agent faces forward

        # Execute command
        if command.action_type == ActionType.MoveForward:
            x += command.magnitude * math.cos(math.radians(angle))
            y += command.magnitude * math.sin(math.radians(angle))
        elif command.action_type == ActionType.TurnLeft:
            angle += command.magnitude * 10  # Turn 10 degrees per unit
        elif command.action_type == ActionType.TurnRight:
            angle -= command.magnitude * 10  # Turn 10 degrees per unit

        # Keep in bounds (will be handled by world)
        new_position = (x, y)
        command.timestamp = self._get_simulation_time() if hasattr(self, '_get_simulation_time') else 0.0

        return {
            "new_position": new_position,
            "angle": angle,
            "command": command,
        }

    def reset(self) -> None:
        """Reset motor controller."""
        self._initialized = False
        self._config = {}
        self._last_command = None

    # Properties for Pythonic interface

    @property
    def is_initialized(self) -> bool:
        """Check if motor controller is initialized."""
        return self._initialized

    @property
    def last_command(self) -> Optional[MotorCommand]:
        """Get last command."""
        return self._last_command

    @property
    def config(self) -> Dict[str, Any]:
        """Get motor controller configuration."""
        return self._config.copy()

    # Internal methods

    def _get_simulation_time(self) -> float:
        """Get current simulation time."""
        return 0.0


def create_motor_command(action_type: ActionType, magnitude: float = 1.0) -> MotorCommand:
    """
    Create a new motor command.
    
    Args:
        action_type: Type of action
        magnitude: Magnitude of action
        
    Returns:
        MotorCommand instance
    """
    return MotorCommand(action_type, magnitude)