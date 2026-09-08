"""
Neuromodulation system module for NLM.

This module provides classes and functions for neuromodulation systems,
including reward modulation, curiosity, novelty detection, and prediction
error computation.
"""

from typing import Dict, Any, Optional, List, Tuple
from enum import Enum
from contextlib import contextmanager
import math

from .exceptions import (
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
    ResourceError,
)

class NeuromodulationSystem:
    """
    Manages neuromodulation systems in NLM.
    
    This class implements neuromodulation systems including reward modulation,
    curiosity-driven exploration, novelty detection, and prediction error
    computation for adaptive learning.
    """

    def __init__(self):
        """Initialize a new NeuromodulationSystem instance."""
        self._initialized = False
        self._config = {}
        self._dopamine_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0

    def __enter__(self):
        """Context manager entry point."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.reset()

    def initialize(self, config: Optional[Dict[str, Any]] = None) -> None:
        """
        Initialize the neuromodulation system.
        
        Args:
            config: Optional configuration dictionary
        """
        self._config = config or {}
        self._initialized = True

    def update(self, timestep: float, reward: float = 0.0) -> Dict[str, Any]:
        """
        Update neuromodulation system.
        
        Args:
            timestep: Time step for update
            reward: Reward signal
            
        Returns:
            Dictionary with neuromodulation results
        """
        if not self._initialized:
            raise InitializationError("NeuromodulationSystem not initialized. Call initialize() first.")

        # Update neuromodulation levels
        self._dopamine_level = self._compute_dopamine_level(reward)
        self._curiosity_level = self._compute_curiosity_level(timestep)
        self._novelty_level = self._compute_novelty_level()
        self._prediction_error = self._compute_prediction_error(reward)

        return {
            "dopamine": self._dopamine_level,
            "curiosity": self._curiosity_level,
            "novelty": self._novelty_level,
            "prediction_error": self._prediction_error,
        }

    def reset(self) -> None:
        """Reset neuromodulation system."""
        self._initialized = False
        self._config = {}
        self._dopamine_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0

    # Properties for Pythonic interface

    @property
    def is_initialized(self) -> bool:
        """Check if neuromodulation system is initialized."""
        return self._initialized

    @property
    def dopamine_level(self) -> float:
        """Get dopamine level."""
        return self._dopamine_level

    @property
    def curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._curiosity_level
n
    @property
    def novelty_level(self) -> float:
        """Get novelty level."""
        return self._novelty_level

    @property
    def prediction_error(self) -> float:
        """Get prediction error."""
        return self._prediction_error

    @property
    def config(self) -> Dict[str, Any]:
        """Get neuromodulation system configuration."""
        return self._config.copy()

    # Computation methods

    def _compute_dopamine_level(self, reward: float) -> float:
        """Compute dopamine level based on reward."""
        scale = self._config.get("neuromod", {}).get("dopamine", {}).get("scale", 1.0)
        return reward * scale

    def _compute_curiosity_level(self, timestep: float) -> float:
        """Compute curiosity level."""
        enable = self._config.get("neuromod", {}).get("curiosity", {}).get("enable", True)
        
        if not enable:
            return 0.0

        # Simple curiosity computation
        return min(1.0, timestep / 100.0)

    def _compute_novelty_level(self) -> float:
        """Compute novelty level."""
        enable = self._config.get("neuromod", {}).get("novelty", {}).get("enable", True)
        
        if not enable:
            return 0.0

        # Simple novelty computation
        return 0.5

    def _compute_prediction_error(self, reward: float) -> float:
        """Compute prediction error."""
        predicted_reward = self._config.get("neuromod", {}).get("predicted_reward", 0.0)
        return abs(reward - predicted_reward)


def create_neuromodulation_system(config: Optional[Dict[str, Any]] = None) -> NeuromodulationSystem:
    """
    Create a new NeuromodulationSystem instance.
    
    Args:
        config: Optional configuration dictionary
        
    Returns:
        NeuromodulationSystem instance
    """
    system = NeuromodulationSystem()
    system.initialize(config)
    return system