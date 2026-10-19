"""
Brain state inspection commands for the Advanced CLI.
"""

import logging
from pathlib import Path
from typing import Optional

from ..api import AdvancedAPI

logger = logging.getLogger(__name__)

class BrainStateCommands:
    """
    Brain state inspection commands.
    
    This class provides brain state inspection commands including:
    - Detailed brain state dump (neurons, synapses, regions)
    - Memory system statistics (working, episodic, semantic, procedural, associative)
    - Neuromodulation system status (dopamine, curiosity, novelty, prediction error)
    - Development system status (current stage, plasticity rates)
    - Prediction system status (prediction error, confidence)
    """
    
    def __init__(self, api: AdvancedAPI):
        """
        Initialize the brain state commands.
        
        Args:
            api: Advanced API instance
        """
        self.api = api
    
    def dump_state(self, output_path: Optional[str] = None) -> str:
        """
        Detailed brain state dump (neurons, synapses, regions).
        
        Args:
            output_path: Output file path
            
        Returns:
            Success message or error
        """
        return self.api.brain_state_dump(output_path)
    
    def memory_statistics(self) -> str:
        """Memory system statistics."""
        return self.api.memory_system_statistics()
    
    def neuromodulation_status(self) -> str:
        """Neuromodulation system status."""
        return self.api.neuromodulation_system_status()
    
    def development_status(self) -> str:
        """Development system status."""
        return self.api.development_system_status()
    
    def prediction_status(self) -> str:
        """Prediction system status."""
        return self.api.prediction_system_status()
