"""
Brain interface module for agent-brain interaction.

This module provides interfaces for agent-brain interaction, including
sensory processing, motor decoding, and neuromodulation systems.
"""

from typing import Dict, Any, Optional, List
from contextlib import contextmanager
import numpy as np

from .exceptions import (
    NLMError,
    InitializationError,
    SimulationError,
)

from .world import SensoryPercept
class BrainInterface:
    """
    Interface for agent-brain interaction.
    
    This class provides a high-level interface for agent-brain interaction,
    including sensory processing, motor decoding, and neuromodulation.
    """

    def __init__(self, brain):
        """
        Initialize BrainInterface.
        
        Args:
            brain: Brain instance to interface with
        """
        self._brain = brain
        self._initialized = False
        self._sensory_buffer = []
        self._motor_buffer = []
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False

    def __enter__(self):
        """Context manager entry point."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.reset()

    def initialize(self, world) -> None:
        """
        Initialize the brain interface with a world.
        
        Args:
            world: World instance to interface with
            
        Raises:
            InitializationError: If initialization fails
        """
        self._world = world
        self._initialized = True

    def process_sensory_input(self, percept: SensoryPercept) -> Dict[str, Any]:
        """
        Process sensory input from the world.
        
        Args:
            percept: Sensory percept from world
            
        Returns:
            Dictionary with processed sensory information
            
        Raises:
            SimulationError: If sensory processing fails
        """
        if not self._initialized:
            raise SimulationError("BrainInterface not initialized. Call initialize() first.")

        # Process sensory data
        self._sensory_buffer.append(percept)
        return self._process_sensory_percept(percept)

    def decode_motor_command(self) -> Dict[str, Any]:
        """
        Decode motor command from brain activity.
        
        Returns:
            Dictionary with motor command information
            
        Raises:
            SimulationError: If motor decoding fails
        """
        if not self._initialized:
            raise SimulationError("BrainInterface not initialized. Call initialize() first.")

        # Decode motor command from brain state
        return self._decode_motor_command()

    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0) -> Dict[str, Any]:
        """
        Apply reward modulation to brain.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
            
        Returns:
            Dictionary with neuromodulation results
            
        Raises:
            SimulationError: If reward modulation fails
        """
        if not self._initialized:
            raise SimulationError("BrainInterface not initialized. Call initialize() first.")

        # Apply reward modulation
        return self._apply_reward_modulation(reward, predicted_reward)

    def update_development(self, timestep: float) -> Dict[str, Any]:
        """
        Update brain development.
        
        Args:
            timestep: Time step for development update
            
        Returns:
            Dictionary with development results
            
        Raises:
            SimulationError: If development update fails
        """
        if not self._initialized:
            raise SimulationError("BrainInterface not initialized. Call initialize() first.")

        # Update development
        return self._update_development(timestep)

    def get_curiosity_level(self) -> float:
        """Get current curiosity level."""
        return self._config.get("neuromod", {}).get("curiosity", {}).get("level", 0.0)

    def get_novelty_level(self) -> float:
        """Get current novelty level."""
        return self._config.get("neuromod", {}).get("novelty", {}).get("level", 0.0)

    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._config.get("neuromod", {}).get("prediction_error", 0.0)

    def get_neuromodulation_level(self) -> float:
        """Get neuromodulation level."""
        return self._config.get("neuromod", {}).get("level", 0.0)

    # Subsystem control

    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation."""
        self._reward_modulation_enabled = enable

    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity."""
        self._structural_plasticity_enabled = enable

    def enable_development(self, enable: bool) -> None:
        """Enable or disable development."""
        self._development_enabled = enable

    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity-driven exploration."""
        self._curiosity_enabled = enable

    # Internal methods

    def _process_sensory_percept(self, percept: SensoryPercept) -> Dict[str, Any]:
        """Process sensory percept."""
        return {
            "processed": True,
            "features": percept.get_features() if hasattr(percept, 'get_features') else [],
        }

    def _decode_motor_command(self) -> Dict[str, Any]:
        """Decode motor command from brain state."""
        return {
            "action_type": "move_forward",
            "magnitude": 1.0,
            "timestamp": self._brain.sim_time if hasattr(self._brain, 'sim_time') else 0.0,
        }

    def _apply_reward_modulation(self, reward: float, predicted_reward: float) -> Dict[str, Any]:
        """Apply reward modulation."""
        return {
            "reward": reward,
            "predicted_reward": predicted_reward,
            "modulation": reward * predicted_reward,
        }

    def _update_development(self, timestep: float) -> Dict[str, Any]:
        """Update development."""
        return {
            "timestep": timestep,
            "development_stage": self._brain.get_developmental_stage() if hasattr(self._brain, 'get_developmental_stage') else "initial",
        }

    def reset(self) -> None:
        """Reset brain interface."""
        self._initialized = False
        self._sensory_buffer.clear()
        self._motor_buffer.clear()
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False

    # Properties for Pythonic interface

    @property
    def is_initialized(self) -> bool:
        """Check if brain interface is initialized."""
        return self._initialized

    @property
    def config(self) -> Dict[str, Any]:
        """Get brain interface configuration."""
        return {
            "reward_modulation_enabled": self._reward_modulation_enabled,
            "structural_plasticity_enabled": self._structural_plasticity_enabled,
            "development_enabled": self._development_enabled,
            "curiosity_enabled": self._curiosity_enabled,
        }
def create_agent_brain(brain) -> BrainInterface:
    """
    Create an AgentBrain interface for brain interaction.
    
    Args:
        brain: Brain instance to create interface for
        
    Returns:
        BrainInterface instance
    """
    return BrainInterface(brain)