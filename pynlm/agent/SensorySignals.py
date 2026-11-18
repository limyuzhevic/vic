'''Sensory signal classes for pynlm'''

from __future__ import annotations

from typing import List, Optional

from .core.types import Timestamp
from .sensory import SensoryInput


class InternalSignals(SensoryInput):
    """Internal signals sensory input for body state.
    
    The InternalSignals class represents internal physiological signals
    such as energy, health, and other body state information.
    """
    
    def __init__(self):
        """Initialize InternalSignals instance."""
        super().__init__()
        self.type = "internal"
        self.dimensions = 16  # Number of internal signal channels
        
        # Initialize internal signals
        self.data = [0.0] * self.dimensions
    
    def addSignal(self, value: float):
        """Add an internal signal value.
        
        Args:
            value: Signal value to add.
        """
        # Add signal to first available slot
        for i, existing_value in enumerate(self.data):
            if existing_value == 0.0:
                self.data[i] = value
                return
        
        # If no slot available, add to end (simple implementation)
        if len(self.data) < self.dimensions:
            self.data.append(value)
            self.dimensions += 1
    
    def getAllSignals(self) -> List[float]:
        """Get all internal signals.
        
        Returns:
            List of all internal signal values.
        """
        return self.data.copy()
    
    def __repr__(self) -> str:
        """Get string representation of internal signals.
        
        Returns:
            String representation of internal signals.
        """
        active_signals = sum(1 for v in self.data if v != 0.0)
        return f"<InternalSignals (active: {active_signals}, " \
               f"timestamp: {self.timestamp:.2f}s)>"


__all__ = ['InternalSignals']