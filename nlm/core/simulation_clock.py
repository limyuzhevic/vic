"""
Simulation clock for NLM.
"""

from typing import Optional
import time

class SimulationClock:
    """
    Clock for NLM neural simulation.
    
    Provides timing information for neural simulation steps.
    """
    
    def __init__(self, timestep: float = 0.001):
        """
        Initialize simulation clock.
        
        Args:
            timestep: Time step for simulation (seconds)
        """
        self.timestep = timestep
        self.current_time = 0.0
        self.current_step = 0
        self.start_time = time.time()
    
    def advance(self, steps: int = 1) -> None:
        """
        Advance the clock.
        
        Args:
            steps: Number of time steps to advance
        """
        self.current_step += steps
        self.current_time += steps * self.timestep
    
    def get_elapsed_time(self) -> float:
        """
        Get elapsed simulation time.
        
        Returns:
            Elapsed time in seconds
        """
        return self.current_time
    
    def get_elapsed_wall_time(self) -> float:
        """
        Get elapsed wall-clock time.
        
        Returns:
            Elapsed wall-clock time in seconds
        """
        return time.time() - self.start_time
    
    def reset(self) -> None:
        """Reset the clock."""
        self.current_time = 0.0
        self.current_step = 0
        self.start_time = time.time()
    
    def sync(self, external_time: float) -> None:
        """
        Sync clock to external time.
        
        Args:
            external_time: External time value
        """
        self.current_time = external_time
        self.current_step = int(external_time / self.timestep)
    
    def __repr__(self) -> str:
        return f"<SimulationClock step={self.current_step}, time={self.current_time:.3f}s>"

__all__ = ['SimulationClock']
