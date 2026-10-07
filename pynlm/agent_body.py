# Agent body implementation for NLM Python API

"""
Agent body state for NLM Python API

Represents the physical state of an agent in the NLM simulation:
position, orientation, velocity, and physiological state.
"""

from typing import Any, Optional
from dataclasses import dataclass, field

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class AgentBodyError(NLMError):
    """Raised when agent body operations fail"""
    pass

class AgentBody:
    """
    Agent body state
    
    Contains position, orientation, velocity, and physiological state
    of an agent in the NLM simulation.
    
    Attributes:
        x: X coordinate (in world cells)
        y: Y coordinate (in world cells)
        orientation: Orientation in degrees (0 = right, 90 = up)
        velocityX: X velocity (cells per second)
        velocityY: Y velocity (cells per second)
        angularVelocity: Angular velocity (degrees per second)
        energy: Current energy level (0-100+)
        health: Current health level (0-100+)
        age: Agent age in seconds
        isMoving: Whether agent is currently moving
        isTurning: Whether agent is currently turning
        lastActionTime: Time of last action (simulation time)
    """
    
    def __init__(self):
        """
        Initialize agent body state.
        """
        # Note: In actual implementation, this would wrap C++ AgentBody object
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 0.0
        self.health = 0.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
        self._initialized = True
    
    def get_position(self) -> tuple[float, float]:
        """
        Get agent position.
        
        Returns:
            (x, y) tuple of position
            
        Examples:
            >>> body = AgentBody()
            >>> x, y = body.getPosition()
            >>> print(f"Position: ({x}, {y})")
        """
        return (self.x, self.y)
    
    def set_position(self, x: float, y: float) -> None:
        """
        Set agent position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Examples:
            >>> body = AgentBody()
            >>> body.setPosition(5.0, 3.0)
        """
        self.x = x
        self.y = y
    
    def get_direction(self) -> tuple[float, float]:
        """
        Get agent direction as unit vector.
        
        Returns:
            (dx, dy) unit vector pointing in agent's direction
            
        Examples:
            >>> body = AgentBody()
            >>> body.setOrientation(45.0)  # Northeast
            >>> dx, dy = body.getDirection()
            >>> print(f"Direction: ({dx:.3f}, {dy:.3f})")
        """
        import math
        angle_rad = math.radians(self.orientation)
        return (math.cos(angle_rad), math.sin(angle_rad))
    
    def set_orientation(self, angle_degrees: float) -> None:
        """
        Set agent orientation.
        
        Args:
            angle_degrees: Orientation in degrees (0 = right, 90 = up)
            
        Examples:
            >>> body = AgentBody()
            >>> body.setOrientation(90.0)  # Face up
        """
        self.orientation = angle_degrees
    
    def get_velocity(self) -> tuple[float, float]:
        """
        Get agent velocity.
        
        Returns:
            (vx, vy) velocity vector
            
        Examples:
            >>> body = AgentBody()
            >>> body.setVelocity(1.0, 0.5)
            >>> vx, vy = body.getVelocity()
        """
        return (self.velocityX, self.velocityY)
    
    def set_velocity(self, vx: float, vy: float) -> None:
        """
        Set agent velocity.
        
        Args:
            vx: X velocity
            vy: Y velocity
            
        Examples:
            >>> body = AgentBody()
            >>> body.setVelocity(0.5, 0.0)  # Move right
        """
        self.velocityX = vx
        self.velocityY = vy
    
    def get_speed(self) -> float:
        """
        Get agent speed.
        
        Returns:
            Speed (magnitude of velocity)
            
        Examples:
            >>> body = AgentBody()
            >>> body.setVelocity(3.0, 4.0)
            >>> speed = body.getSpeed()
            >>> print(f"Speed: {speed}")  # Should be 5.0
        """
        return (self.velocityX ** 2 + self.velocityY ** 2) ** 0.5
    
    def get_facing_direction(self) -> float:
        """
        Get agent facing direction.
        
        Returns:
            Facing direction in degrees (0 = right, 90 = up)
            
        Examples:
            >>> body = AgentBody()
            >>> direction = body.getFacingDirection()
            >>> print(f"Facing: {direction}°")
        """
        return self.orientation
    
    def is_at_position(self, x: float, y: float, tolerance: float = 0.1) -> bool:
        """
        Check if agent is at given position within tolerance.
        
        Args:
            x: X coordinate to check
            y: Y coordinate to check
            tolerance: Position tolerance
            
        Returns:
            True if agent is at position within tolerance
            
        Examples:
            >>> body = AgentBody()
            >>> body.setPosition(5.0, 3.0)
            >>> if body.isAtPosition(5.0, 3.0, 0.1):
            ...     print("Agent is at target")
        """
        return ((self.x - x) ** 2 + (self.y - y) ** 2) ** 0.5 <= tolerance
    
    def distance_to(self, other_x: float, other_y: float) -> float:
        """
        Get distance to other position.
        
        Args:
            other_x: Other X coordinate
            other_y: Other Y coordinate
            
        Returns:
            Distance to other position
            
        Examples:
            >>> body = AgentBody()
            >>> body.setPosition(1.0, 2.0)
            >>> distance = body.distanceTo(4.0, 6.0)
            >>> print(f"Distance: {distance}")  # Should be 5.0
        """
        return ((self.x - other_x) ** 2 + (self.y - other_y) ** 2) ** 0.5
    
    def move_to(self, x: float, y: float, speed: float = 1.0, duration: float = 1.0) -> None:
        """
        Move agent to target position.
        
        Args:
            x: Target X coordinate
            y: Target Y coordinate
            speed: Movement speed
            duration: Movement duration
            
        Examples:
            >>> body = AgentBody()
            >>> body.moveTo(10.0, 10.0, speed=2.0, duration=5.0)
        """
        dx = x - self.x
        dy = y - self.y
        distance = (dx ** 2 + dy ** 2) ** 0.5
        
        if distance > 0:
            self.set_orientation(((180 / 3.14159) * ((dy / distance) * 3.14159)) % 360)
            self.set_velocity((dx / distance) * speed, (dy / distance) * speed)
    
    def rotate_to(self, angle_degrees: float, angular_speed: float = 1.0, duration: float = 1.0) -> None:
        """
        Rotate agent to target orientation.
        
        Args:
            angle_degrees: Target orientation in degrees
            angular_speed: Angular speed (degrees per second)
            duration: Rotation duration
            
        Examples:
            >>> body = AgentBody()
            >>> body.rotateTo(90.0, angularSpeed=45.0, duration=2.0)
        """
        current = self.orientation
        target = angle_degrees
        
        # Handle angle wrapping
        diff = target - current
        if diff > 180:
            diff -= 360
        elif diff < -180:
            diff += 360
        
        needed_rotation = abs(diff)
        if needed_rotation > 0:
            actual_rotation = angular_speed * duration
            self.orientation = current + (diff / needed_rotation) * actual_rotation
            self.orientation = self.orientation % 360
    
    def reset(self) -> None:
        """
        Reset agent body to default state.
        
        Examples:
            >>> body = AgentBody()
            >>> body.reset()
        """
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 0.0
        self.health = 0.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def __repr__(self) -> str:
        """String representation of agent body"""
        return f"<AgentBody pos=({self.x:.1f}, {self.y:.1f}), dir={self.orientation:.1f}°, speed={self.getSpeed():.2f}, energy={self.energy:.1f}>"
    
    def __str__(self) -> str:
        """String representation of agent body"""
        return self.__repr__()

# Factory function for agent body creation
def create_agent_body() -> AgentBody:
    """
    Create an agent body.
    
    Returns:
        New AgentBody instance
        
    Examples:
        >>> body = createAgentBody()
    """
    return AgentBody()