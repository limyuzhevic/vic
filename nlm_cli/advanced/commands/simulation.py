"""
Simulation control commands for the Advanced CLI.
"""

import logging
from typing import Optional

from ..api import AdvancedAPI

logger = logging.getLogger(__name__)

class SimulationCommands:
    """
    Simulation control commands.
    
    This class provides simulation control commands including:
    - Speed control (slow motion, fast forward)
    - Pause/resume simulation
    - Jump to specific steps
    - Record simulation data
    - Replay recorded simulations
    """
    
    def __init__(self, api: AdvancedAPI):
        """
        Initialize the simulation commands.
        
        Args:
            api: Advanced API instance
        """
        self.api = api
    
    def control_speed(self, mode: str, factor: float = 1.0) -> str:
        """
        Control simulation speed (slow motion, fast forward).
        
        Args:
            mode: Speed mode
            factor: Speed factor
            
        Returns:
            Success message or error
        """
        return self.api.control_simulation_speed(mode, factor)
    
    def pause(self) -> str:
        """Pause simulation."""
        return self.api.pause_simulation()
    
    def resume(self) -> str:
        """Resume simulation."""
        return self.api.resume_simulation()
    
    def jump_to_step(self, step: int) -> str:
        """
        Jump to specific step.
        
        Args:
            step: Step number to jump to
            
        Returns:
            Success message or error
        """
        return self.api.jump_to_step(step)
    
    def record(self, output_path: str) -> str:
        """
        Record simulation data.
        
        Args:
            output_path: Output file path
            
        Returns:
            Success message or error
        """
        return self.api.record_simulation(output_path)
    
    def replay(self, input_path: str) -> str:
        """
        Replay recorded simulation.
        
        Args:
            input_path: Input file path
            
        Returns:
            Success message or error
        """
        return self.api.replay_simulation(input_path)
