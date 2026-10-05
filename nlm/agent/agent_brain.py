"""
AgentBrain class for NLM neural simulation framework.

This module provides a Python interface to the C++ AgentBrain class with enhanced
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
)

# Forward references
Brain = Any
SimpleWorld = Any
NeuronId = Any
NeuronType = Any
ActionType = Any
MotorCommand = Any
DevelopmentalStage = Any
SensoryPercept = Any
Config = Any

@dataclass
class AgentBrainStatus:
    """Status information for agent brain simulation"""
    reward_modulation_enabled: bool = False
    structural_plasticity_enabled: bool = False
    development_enabled: bool = False
    curiosity_enabled: bool = False
    neuromodulation_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    developmental_stage: DevelopmentalStage = DevelopmentalStage.Initial
    last_update_time: float = 0.0

class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world.
    
    Handles sensory transduction and motor decoding for agent simulation.
    
    This class provides a Python interface to the underlying C++ AgentBrain class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, brain: Brain = None, world: SimpleWorld = None):
        """
        Initialize agent brain with brain and world integration.
        
        Args:
            brain: Brain object (optional)
            world: World object (optional)
        """
        self._brain = brain
        self._world = world
        self._logger = PythonSessionLogger()
        self._status = AgentBrainStatus()
        
        # Motor neuron groups (placeholders)
        self._motor_forward = []
        self._motor_backward = []
        self._motor_turn_left = []
        self._motor_turn_right = []
        self._motor_interact = []
        self._motor_wait = []
        
        # Sensory neuron groups (placeholders)
        self._sensory_vision = []
        self._sensory_touch = []
        self._sensory_internal = []
        self._sensory_proprioception = []
        
        # Neuromodulation state
        self._dopamine_level = 0.0
        self._novelty_level = 0.0
        self._curiosity_level = 0.0
        self._prediction_error = 0.0
        self._expected_reward = 0.0
        
        # Development state
        self._developmental_age = 0.0
        self._plasticity_modifier = 1.0
        
        # Configuration flags
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        
        # Previous sensory state for novelty detection
        self._previous_vision = []
        self._sensory_novelty_decay = 0.01
        
        # Log initialization
        self._logger.logEntry(f"AgentBrain initialized with brain={brain is not None}, world={world is not None}")
    
    def initialize(self, world: SimpleWorld = None) -> bool:
        """
        Initialize with world.
        
        Args:
            world: World object (optional if already provided in constructor)
            
        Returns:
            True if successful, False otherwise
        """
        if world is not None:
            self._world = world
        
        if self._world is None:
            self._format_python_error("AgentBrain must have a world for initialization")
            return False
        
        try:
            # Initialize C++ agent brain (placeholder)
            # self._cpp_agent_brain = create_cpp_agent_brain(self._brain, self._world)
            
            self._logger.logEntry("AgentBrain initialized successfully")
            return True
            
        except Exception as e:
            self._format_python_error(f"AgentBrain initialization failed: {e}")
            self._logger.logEntry(f"AgentBrain initialization failed: {e}")
            return False
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size."""
        # Placeholder - should be configured
        return 10
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size."""
        # Placeholder - should be configured
        return 10
    
    def processSensoryInput(self, percept: SensoryPercept) -> bool:
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if self._brain is None:
                self._format_python_error("AgentBrain must have a brain for processing sensory input")
                return False
            
            # Convert percept to C++ format (placeholder)
            # self._cpp_agent_brain.processSensoryInput(percept)
            
            # Store previous vision for novelty detection
            if hasattr(percept, 'getVision'):
                vision_data = percept.getVision()
                if isinstance(vision_data, np.ndarray):
                    self._previous_vision = vision_data.copy()
            
            self._logger.logEntry(f"Processed sensory input: {type(percept).__name__}")
            
            return True
            
        except Exception as e:
            self._format_python_error(f"Sensory input processing failed: {e}")
            self._logger.logEntry(f"Sensory input processing failed: {e}")
            return False
    
    def decodeMotorCommand(self) -> MotorCommand:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            MotorCommand object
        """
        try:
            # Decode motor command from C++ brain (placeholder)
            # motor_command = self._cpp_agent_brain.decodeMotorCommand()
            
            # Calculate novelty from sensory changes
            self._calculate_novelty()
            
            # Apply curiosity weighting
            motor_command = self._select_with_curiosity(MotorCommand.Wait)
            
            self._logger.logEntry(f"Decoded motor command: {type(motor_command).__name__}")
            
            return motor_command
            
        except Exception as e:
            self._format_python_error(f"Motor command decoding failed: {e}")
            self._logger.logEntry(f"Motor command decoding failed: {e}")
            
            # Return default command on error
            return MotorCommand.Wait
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Expected reward
        """
        try:
            if not self._reward_modulation_enabled:
                return
            
            # Calculate reward prediction error
            reward_error = reward - predicted_reward
            
            # Apply to dopamine system (placeholder)
            self._dopamine_level = max(0.0, min(1.0, 0.5 + reward_error * 0.1))
            
            # Update status
            self._status.neuromodulation_level = self._dopamine_level
            self._status.prediction_error = abs(reward_error)
            self._expected_reward = predicted_reward
            
            self._logger.logEntry(f"Applied reward modulation: reward={reward}, predicted={predicted_reward}, error={reward_error}")
            
        except Exception as e:
            self._format_python_error(f"Reward modulation failed: {e}")
    
    def updateDevelopment(self, timestep: float) -> None:
        """
        Update development system.
        
        Args:
            timestep: Time step for development update
        """
        try:
            if not self._development_enabled:
                return
            
            # Update developmental age
            self._developmental_age += timestep
            
            # Update plasticity modifier
            self._plasticity_modifier = max(0.1, min(2.0, 1.0 + (self._developmental_age / 1000.0)))
            
            # Update status
            self._status.last_update_time = time.time()
            
            self._logger.logEntry(f"Updated development: age={self._developmental_age}, plasticity={self._plasticity_modifier}")
            
        except Exception as e:
            self._format_python_error(f"Development update failed: {e}")
    
    # Configuration methods
    
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable or disable reward modulation."""
        self._reward_modulation_enabled = enable
        self._status.reward_modulation_enabled = enable
        self._logger.logEntry(f"Reward modulation {'enabled' if enable else 'disabled'}")
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity."""
        self._structural_plasticity_enabled = enable
        self._status.structural_plasticity_enabled = enable
        self._logger.logEntry(f"Structural plasticity {'enabled' if enable else 'disabled'}")
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable or disable development."""
        self._development_enabled = enable
        self._status.development_enabled = enable
        self._logger.logEntry(f"Development {'enabled' if enable else 'disabled'}")
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable or disable curiosity."""
        self._curiosity_enabled = enable
        self._status.curiosity_enabled = enable
        self._logger.logEntry(f"Curiosity {'enabled' if enable else 'disabled'}")
    
    # Status query methods
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        return self._status.neuromodulation_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        return self._curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        return self._novelty_level
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        return self._status.prediction_error
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._status.developmental_stage
    
    def getBrain(self) -> Brain:
        """Get brain pointer."""
        return self._brain
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        try:
            # Reset C++ agent brain (placeholder)
            # self._cpp_agent_brain.reset()
            
            # Reset state
            self._reward_modulation_enabled = False
            self._structural_plasticity_enabled = False
            self._development_enabled = False
            self._curiosity_enabled = False
            
            self._dopamine_level = 0.0
            self._novelty_level = 0.0
            self._curiosity_level = 0.0
            self._prediction_error = 0.0
            self._expected_reward = 0.0
            
            self._developmental_age = 0.0
            self._plasticity_modifier = 1.0
            
            # Reset status
            self._status = AgentBrainStatus()
            
            self._logger.logEntry("AgentBrain reset")
            
        except Exception as e:
            self._format_python_error(f"AgentBrain reset failed: {e}")
    
    def getConfiguration(self) -> Config:
        """Get brain configuration."""
        if self._brain is None:
            self._format_python_error("AgentBrain must have a brain for configuration access")
            return None
        
        return self._brain.getConfig()
    
    def createWithDefaults(self, brain_name: str) -> 'AgentBrain':
        """
        Create agent brain with default configuration.
        
        Args:
            brain_name: Name for the brain
            
        Returns:
            New AgentBrain instance
        """
        # Create configuration
        from nlm.core import Config
        
        config = Config()
        config.set("brain_name", brain_name)
        config.set("simulation_steps", 1000)
        config.set("enable_reward_modulation", True)
        config.set("enable_structural_plasticity", True)
        
        # Create brain (placeholder)
        # brain = Brain(config)
        brain = None
        
        # Create agent brain
        agent_brain = AgentBrain(brain)
        agent_brain.enableRewardModulation(True)
        agent_brain.enableStructuralPlasticity(True)
        
        return agent_brain
    
    def __repr__(self) -> str:
        """String representation of agent brain."""
        return f"<AgentBrain: neuromodulation={self._status.neuromodulation_level:.2f}>"
    
    def __str__(self) -> str:
        """String representation of agent brain."""
        return f"NLM Agent Brain with neuromodulation system"
    
    # Helper methods
    
    def _calculate_novelty(self) -> None:
        """Calculate novelty from sensory changes."""
        try:
            # Simple novelty calculation based on vision changes
            current_vision = getattr(self, '_current_vision', [])
            if current_vision and self._previous_vision:
                if isinstance(current_vision, np.ndarray) and isinstance(self._previous_vision, np.ndarray):
                    # Calculate difference
                    diff = np.abs(current_vision - self._previous_vision)
                    novelty = np.mean(diff)
                    
                    # Apply decay
                    self._novelty_level = novelty * (1.0 - self._sensory_novelty_decay)
                    
                    # Store previous
                    self._previous_vision = current_vision.copy()
        except Exception:
            self._novelty_level = 0.0
    
    def _select_with_curiosity(self, default_cmd: MotorCommand) -> MotorCommand:
        """
        Select motor command with curiosity/exploration.
        
        Args:
            default_cmd: Default motor command
            
        Returns:
            Selected motor command
        """
        try:
            if self._curiosity_enabled and self._curiosity_level > 0.5:
                # Simple curiosity-driven exploration
                # In real implementation, this would use more sophisticated selection
                self._curiosity_level *= 0.9  # Decay curiosity
                return MotorCommand.Look  # Default exploration command
            else:
                return default_cmd
        except Exception:
            return default_cmd
    
    def _format_python_error(self, error_msg: str) -> None:
        """Format Python error with traceback for better debugging."""
        try:
            # Format traceback for better debugging
            import traceback
            traceback.print_exc()
        except Exception:
            print(f"Error: {error_msg}")

__all__ = ['AgentBrain']
