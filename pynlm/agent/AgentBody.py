'''Agent body state for pynlm'''

from __future__ import annotations


class AgentBody:
    """Agent body state - contains physical properties and internal state.
    
    This class represents the agent's physical body state in the world,
    including position, velocity, orientation, and physiological state.
    """
    
    def __init__(self):
        """Initialize an AgentBody instance."""
        # Position in world
        self.x: float = 0.0
        self.y: float = 0.0
        
        # Orientation (radians)
        self.orientation: float = 0.0
        
        # Velocity
        self.velocityX: float = 0.0
        self.velocityY: float = 0.0
        self.angularVelocity: float = 0.0
        
        # Internal state
        self.energy: float = 1.0  # Energy level (0-1)
        self.health: float = 1.0  # Health (0-1)
        self.age: float = 0.0  # Developmental age in simulation steps
        
        # Movement state
        self.isMoving: bool = False
        self.isTurning: bool = False
        self.lastActionTime: float = -1000.0
    
    def __repr__(self) -> str:
        """Get string representation of the agent body.
        
        Returns:
            String representation of the agent body.
        """
        return f"<AgentBody (pos: ({self.x:.1f}, {self.y:.1f}), " \
               f"orientation: {self.orientation:.2f}, energy: {self.energy:.2f})>"


__all__ = ['AgentBody']