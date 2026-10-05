"""
SimpleWorld class for NLM neural simulation framework.

This module provides a Python interface to the C++ SimpleWorld class with enhanced
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
    numpy_utils,
    numpy_export,
)

# Forward references
AgentBody = Any
ActionType = Any
MotorCommand = Any
SensoryPercept = Any
Config = Any
WorldObject = Any

@dataclass
class WorldObjectData:
    """Data structure for world objects"""
    x: float = 0.0
    y: float = 0.0
    radius: float = 0.5
    type: str = "Empty"
    value: float = 0.0
    active: bool = True

class SimpleWorld:
    """
    Simple 2D world for NLM simulation.
    
    This class provides a Python interface to the underlying C++ SimpleWorld class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self):
        """Initialize simple world."""
        self._logger = PythonSessionLogger()
        
        # World dimensions
        self._width = 100.0
        self._height = 100.0
        
        # Vision settings
        self._vision_width = 84
        self._vision_height = 84
        
        # Agent state
        self._agent_body = AgentBody()
        self._agent_start_position = (0.0, 0.0)
        
        # World objects (storage for world objects)
        self._world_objects = []
        
        # World configuration
        self._max_energy = 100.0
        self._energy_decay_rate = 0.01
        
        # Simulation state
        self._simulation_time = 0.0
        self._time_step = 0.1
        self._random_seed = 42
        
        # Log initialization
        self._logger.logEntry("SimpleWorld initialized")
    
    def configure(self, width: float, height: float, vision_width: int, vision_height: int) -> None:
        """
        Configure world dimensions and vision settings.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        try:
            # Validate dimensions
            if width <= 0 or height <= 0:
                self._format_python_error("World dimensions must be positive")
                return
            
            if vision_width <= 0 or vision_height <= 0:
                self._format_python_error("Vision dimensions must be positive")
                return
            
            # Set dimensions
            self._width = width
            self._height = height
            self._vision_width = vision_width
            self._vision_height = vision_height
            
            self._logger.logEntry(f"World configured: {width}x{height}, vision={vision_width}x{vision_height}")
            
        except Exception as e:
            self._format_python_error(f"World configuration failed: {e}")
    
    def reset(self) -> None:
        """Reset world state."""
        try:
            # Reset agent
            self._agent_body = AgentBody()
            self._agent_body.x, self._agent_body.y = self._agent_start_position
            
            # Clear world objects
            self._world_objects.clear()
            
            # Reset simulation time
            self._simulation_time = 0.0
            
            self._logger.logEntry("SimpleWorld reset")
            
        except Exception as e:
            self._format_python_error(f"World reset failed: {e}")
    
    def setAgentStart(self, x: float, y: float) -> None:
        """
        Set agent start position.
        
        Args:
            x: X-coordinate
            y: Y-coordinate
        """
        try:
            self._agent_start_position = (x, y)
            self._logger.logEntry(f"Agent start position set: ({x}, {y})")
            
        except Exception as e:
            self._format_python_error(f"Failed to set agent start position: {e}")
    
    def update(self, timestep: float) -> None:
        """
        Update world state.
        
        Args:
            timestep: Time step for update
        """
        try:
            # Update simulation time
            self._simulation_time += timestep
            
            # Apply energy decay
            self._agent_body.energy *= (1.0 - self._energy_decay_rate)
            
            # Update agent velocity (simple physics)
            if self._agent_body.is_moving:
                # Apply friction
                self._agent_body.velocity_x *= 0.95
                self._agent_body.velocity_y *= 0.95
                
                # Update position
                self._agent_body.x += self._agent_body.velocity_x * timestep
                self._agent_body.y += self._agent_body.velocity_y * timestep
                
                # Constrain to world boundaries
                self._constrain_position()
                
                # Update action time
                self._agent_body.last_action_time = self._simulation_time
            
            # Log update
            self._logger.logEntry(f"World updated: time={self._simulation_time}, position=({self._agent_body.x:.2f}, {self._agent_body.y:.2f})")
            
        except Exception as e:
            self._format_python_error(f"World update failed: {e}")
    
    def applyMotorCommand(self, cmd: MotorCommand, current_time: float) -> ActionResult:
        """
        Apply motor command.
        
        Args:
            cmd: Motor command to apply
            current_time: Current simulation time
            
        Returns:
            Action result
        """
        try:
            # Create action result
            result = ActionResult()
            
            # Process motor command
            if cmd == MotorCommand.MoveForward:
                self._agent_body.move_forward()
                result.reward = 1.0
                result.success = True
                result.message = "Moved forward"
                
            elif cmd == MotorCommand.MoveBackward:
                self._agent_body.move_backward()
                result.reward = 0.5
                result.success = True
                result.message = "Moved backward"
                
            elif cmd == MotorCommand.TurnLeft:
                self._agent_body.turn_left()
                result.reward = 0.5
                result.success = True
                result.message = "Turned left"
                
            elif cmd == MotorCommand.TurnRight:
                self._agent_body.turn_right()
                result.reward = 0.5
                result.success = True
                result.message = "Turned right"
                
            elif cmd == MotorCommand.Look:
                self._agent_body.look()
                result.reward = 0.2
                result.success = True
                result.message = "Looking"
                
            elif cmd == MotorCommand.Interact:
                self._agent_body.interact()
                result.reward = 2.0
                result.success = True
                result.message = "Interacted with environment"
                
            elif cmd == MotorCommand.Wait:
                self._agent_body.wait(1.0)
                result.reward = 0.1
                result.success = True
                result.message = "Waited"
                
            else:
                result.success = False
                result.message = "Unknown motor command"
            
            # Check for boundary violations
            if not (0 <= self._agent_body.x <= self._width and 0 <= self._agent_body.y <= self._height):
                result.success = False
                result.reward = -1.0
                result.message = "Out of bounds"
            
            # Log command application
            self._logger.logEntry(f"Applied motor command: {type(cmd).__name__}, result={result.success}, reward={result.reward}")
            
            return result
            
        except Exception as e:
            self._format_python_error(f"Motor command application failed: {e}")
            result = ActionResult()
            result.success = False
            result.reward = -1.0
            result.message = f"Error: {e}"
            return result
    
    def getSensoryPercept(self) -> SensoryPercept:
        """
        Get sensory percept.
        
        Returns:
            Sensory percept
        """
        try:
            # Create sensory percept
            percept = SensoryPercept()
            
            # Set vision data (simple simulation)
            vision_data = np.zeros((self._vision_height, self._vision_width, 3), dtype=np.float32)
            
            # Add agent position information
            agent_rel_x = (self._agent_body.x / self._width) * 2 - 1
            agent_rel_y = (self._agent_body.y / self._height) * 2 - 1
            
            # Fill vision data with simple representation
            center_x = self._vision_width // 2
            center_y = self._vision_height // 2
            
            # Add agent as bright spot
            agent_vision_x = int((agent_rel_x + 1) * self._vision_width / 2)
            agent_vision_y = int((agent_rel_y + 1) * self._vision_height / 2)
            
            if 0 <= agent_vision_x < self._vision_width and 0 <= agent_vision_y < self._vision_height:
                vision_data[agent_vision_y, agent_vision_x] = [1.0, 1.0, 1.0]  # White for agent
            
            # Add random noise for environment
            noise = np.random.normal(0, 0.1, vision_data.shape)
            vision_data = np.clip(vision_data + noise, 0, 1)
            
            # Set vision data
            percept.setVision(vision_data)
            
            # Set audio data (simple simulation)
            audio_data = np.random.normal(0, 0.5, 8000)  # 1 second of audio at 8kHz
            percept.setAudio(audio_data)
            
            # Set timestamp
            percept.setTimestamp(self._simulation_time)
            
            # Log percept creation
            self._logger.logEntry(f"Generated sensory percept: vision={self._vision_width}x{self._vision_height}, audio={len(audio_data)} samples")
            
            return percept
            
        except Exception as e:
            self._format_python_error(f"Failed to generate sensory percept: {e}")
            return SensoryPercept()
    
    def getAgentBody(self) -> AgentBody:
        """
        Get agent body.
        
        Returns:
            Agent body
        """
        return self._agent_body
    
    def addObject(self, obj: WorldObject) -> None:
        """
        Add world object.
        
        Args:
            obj: World object to add
        """
        try:
            self._world_objects.append(obj)
            self._logger.logEntry(f"Added world object: {type(obj).__name__} at ({obj.x}, {obj.y})")
            
        except Exception as e:
            self._format_python_error(f"Failed to add world object: {e}")
    
    def removeObject(self, x: float, y: float) -> bool:
        """
        Remove world object at specified position.
        
        Args:
            x: X-coordinate
            y: Y-coordinate
            
        Returns:
            True if object was removed, False otherwise
        """
        try:
            for i, obj in enumerate(self._world_objects):
                if abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1:
                    del self._world_objects[i]
                    self._logger.logEntry(f"Removed world object at ({x}, {y})")
                    return True
            
            return False
            
        except Exception as e:
            self._format_python_error(f"Failed to remove world object: {e}")
            return False
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """
        Check if position is valid (within world bounds and not occupied).
        
        Args:
            x: X-coordinate
            y: Y-coordinate
            
        Returns:
            True if position is valid, False otherwise
        """
        try:
            # Check bounds
            if x < 0 or x > self._width or y < 0 or y > self._height:
                return False
            
            # Check object collisions (simplified)
            for obj in self._world_objects:
                if abs(obj.x - x) < obj.radius and abs(obj.y - y) < obj.radius:
                    return False
            
            return True
            
        except Exception:
            return False
    
    def getWidth(self) -> float:
        """Get world width."""
        return self._width
    
    def getHeight(self) -> float:
        """Get world height."""
        return self._height
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy."""
        return self._max_energy
    
    def setMaxEnergy(self, e: float) -> None:
        """
        Set maximum energy.
        
        Args:
            e: Maximum energy
        """
        self._max_energy = max(0.0, e)
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate."""
        return self._energy_decay_rate
    
    def setEnergyDecayRate(self, r: float) -> None:
        """
        Set energy decay rate.
        
        Args:
            r: Energy decay rate
        """
        self._energy_decay_rate = max(0.0, min(1.0, r))
    
    def getSimulationTime(self) -> float:
        """Get simulation time."""
        return self._simulation_time
    
    def setRandomSeed(self, seed: int) -> None:
        """
        Set random seed.
        
        Args:
            seed: Random seed
        """
        self._random_seed = seed
        np.random.seed(seed)
    
    def getRandomSeed(self) -> int:
        """Get random seed."""
        return self._random_seed
    
    def getObject(self, index: int) -> Optional[WorldObject]:
        """
        Get world object by index.
        
        Args:
            index: Object index
            
        Returns:
            World object or None if not found
        """
        if 0 <= index < len(self._world_objects):
            return self._world_objects[index]
        return None
    
    # Helper methods
    
    def _constrain_position(self) -> None:
        """Constrain agent position to world boundaries."""
        self._agent_body.x = max(0.0, min(self._width, self._agent_body.x))
        self._agent_body.y = max(0.0, min(self._height, self._agent_body.y))
    
    def _format_python_error(self, error_msg: str) -> None:
        """Format Python error with traceback for better debugging."""
        try:
            # Format traceback for better debugging
            import traceback
            traceback.print_exc()
        except Exception:
            print(f"Error: {error_msg}")
    
    def __repr__(self) -> str:
        """String representation of simple world."""
        return f"<SimpleWorld: {self._width}x{self._height}, time={self._simulation_time:.2f}>"
    
    def __str__(self) -> str:
        """String representation of simple world."""
        return f"NLM Simple World simulation at ({self._width}x{self._height})"

__all__ = ['SimpleWorld']
