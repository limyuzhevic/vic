# Enhanced agent interface for NLM simulations
# This module provides the Pythonic interface to agent brain and action systems

import numpy as np
from typing import Optional, List, Dict, Any, Tuple
from enum import Enum
from .core import NLMError, ValidationError, SimulationError
from .brain import BrainSession
class ActionInterface:
    """
    Enhanced action interface with Pythonic API.
    
    This class provides an enhanced interface to the Action class with
    better type hints, convenience methods, and error handling.
    """
    
    def __init__(self, action_type=None, parameters=None):
        """
        Initialize action interface.
        
        Args:
            action_type: Type of action
            parameters: Action parameters
        """
        self.action_type = action_type
        self.parameters = parameters or {}
        
    @property
    def type(self):
        """Get action type."""
        return self.action_type
        
    @property
    def params(self) -> Dict[str, Any]:
        """Get action parameters."""
        return self.parameters.copy()
        
    def get_parameter(self, key: str, default=None) -> Any:
        """
        Get a specific parameter value.
        
        Args:
            key: Parameter name
            default: Default value if parameter not found
            
        Returns:
            Parameter value or default
        """
        return self.parameters.get(key, default)
        
    def set_parameter(self, key: str, value: Any):
        """
        Set a parameter value.
        
        Args:
            key: Parameter name
            value: Parameter value
        """
        self.parameters[key] = value
        
    def get_name(self) -> str:
        """Get human-readable action name."""
        return str(self.action_type) if self.action_type else "Unknown"
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert action to dictionary."""
        return {
            'type': str(self.action_type) if self.action_type else None,
            'parameters': self.parameters,
            'name': self.get_name(),
        }
        
    def __repr__(self) -> str:
        """String representation of action."""
        return f"Action(type={self.action_type}, params={self.parameters})"
class AgentInterface:
    """
    Enhanced agent interface with Pythonic API.
    
    This class provides an enhanced interface to the AgentBrain class with
    better error handling, convenience methods, and type hints.
    """
    
    def __init__(self, brain=None):
        """
        Initialize agent interface.
        
        Args:
            brain: Brain object to control
        """
        self._brain = brain
        self._initialized = False
        
    def initialize(self, brain=None):
        """
        Initialize agent with brain.
        
        Args:
            brain: Brain object to control
        """
        if brain is not None:
            self._brain = brain
        if self._brain is not None:
            self._initialized = True
        else:
            raise SimulationError("Brain must be provided for agent initialization")
            
    @property
    def is_initialized(self) -> bool:
        """Check if agent is initialized."""
        return self._initialized
        
    def process_sensory_input(self, percept: SensoryData):
        """
        Process sensory input from the world.
        
        Args:
            percept: Sensory data to process
            
        Raises:
            SimulationError: If processing fails
        """
        if not self._initialized:
            raise SimulationError("Agent not initialized. Call initialize() first.")
            
        if not self._brain:
            raise SimulationError("Agent brain not set.")
            
    def decode_motor_command(self) -> ActionInterface:
        """
        Decode brain activity into motor command.
        
        Returns:
            ActionInterface object
            
        Raises:
            SimulationError: If command cannot be decoded
        """
        if not self._initialized:
            raise SimulationError("Agent not initialized. Call initialize() first.")
            
        # This would decode motor command in the actual implementation
        return ActionInterface()
        
    def enable_system(self, system_name: str, enable: bool = True):
        """
        Enable or disable a system.
        
        Args:
            system_name: Name of system ('reward_modulation', 'structural_plasticity', 
                        'development', 'curiosity')
            enable: Whether to enable the system
        """
        if not self._brain or not self._brain.getConfig():
            raise SimulationError("Agent not properly initialized.")
            
        config = self._brain.getConfig()
        system_keys = {
            'reward_modulation': 'neuromodulation.reward.enabled',
            'structural_plasticity': 'neuromodulation.structural_plasticity.enabled',
            'development': 'development.enabled',
            'curiosity': 'neuromodulation.curiosity.enabled',
        }
        
        if system_name in system_keys:
            config.set(system_keys[system_name], enable)
            
    def is_system_enabled(self, system_name: str) -> bool:
        """
        Check if a system is enabled.
        
        Args:
            system_name: Name of system to check
            
        Returns:
            True if system is enabled
        """
        if not self._brain or not self._brain.getConfig():
            return False
            
        config = self._brain.getConfig()
        system_keys = {
            'reward_modulation': 'neuromodulation.reward.enabled',
            'structural_plasticity': 'neuromodulation.structural_plasticity.enabled',
            'development': 'development.enabled',
            'curiosity': 'neuromodulation.curiosity.enabled',
        }
        
        if system_name in system_keys:
            return config.get[bool](system_keys[system_name])
        return False
        
    def get_sensory_input_size(self) -> int:
        """
        Get expected sensory input size.
        
        Returns:
            Expected sensory input size
        """
        if self._brain:
            return self._brain.getSensoryInputSize()
        return 0
        
    def get_motor_output_size(self) -> int:
        """
        Get expected motor output size.
        
        Returns:
            Expected motor output size
        """
        if self._brain:
            return self._brain.getMotorOutputSize()
        return 0
        
    def get_developer_stage(self) -> int:
        """
        Get current developmental stage.
        
        Returns:
            Developmental stage (0-4)
        """
        if self._brain:
            return self._brain.getDevelopmentalStage()
        return 0
        
    def update_development(self, timestep: float):
        """
        Update development system.
        
        Args:
            timestep: Time step duration
        """
        if self._brain:
            self._brain.updateDevelopment(timestep)
            
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0):
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        if self._brain:
            self._brain.applyRewardModulation(reward, predicted_reward)
            
    def get_neuromodulation_level(self) -> float:
        """
        Get current neuromodulation level.
        
        Returns:
            Neuromodulation level
        """
        if self._brain:
            return self._brain.getNeuromodulationLevel()
        return 0.0
        
    def get_curiosity_level(self) -> float:
        """
        Get curiosity level.
        
        Returns:
            Curiosity level
        """
        if self._brain:
            return self._brain.getCuriosityLevel()
        return 0.0
        
    def get_novelty_level(self) -> float:
        """
        Get novelty level.
        
        Returns:
            Novelty level
        """
        if self._brain:
            return self._brain.getNoveltyLevel()
        return 0.0
        
    def get_prediction_error(self) -> float:
        """
        Get prediction error.
        
        Returns:
            Prediction error
        """
        if self._brain:
            return self._brain.getPredictionError()
        return 0.0
        
    def reset(self):
        """Reset agent to initial state."""
        self._initialized = False
        
    def run_episode(self, brain, world, steps: int, 
                    timestep: float = 0.1, **kwargs) -> Dict[str, Any]:
        """
        Run a complete episode.
        
        Args:
            brain: Brain object
            world: World interface
            steps: Number of steps to run
            timestep: Time step duration
            **kwargs: Additional configuration
            
        Returns:
            Dictionary with episode statistics
        """
        # Create agent interface
        agent = AgentInterface(brain)
        agent.initialize(brain)
        
        # Enable specified systems
        for system_name, enabled in kwargs.get('systems', {}).items():
            agent.enable_system(system_name, enabled)
            
        # Run simulation
        session = BrainSession(brain, world, agent)
        stats = session.run_simulation(steps, timestep)
        
        # Add agent-specific statistics
        stats.update({
            'agent_sensory_size': agent.get_sensory_input_size(),
            'agent_motor_size': agent.get_motor_output_size(),
            'agent_development_stage': agent.get_developer_stage(),
            'agent_neuromodulation': agent.get_neuromodulation_level(),
            'agent_curiosity': agent.get_curiosity_level(),
        })
        
        return stats
        
    def __repr__(self) -> str:
        """String representation of agent interface."""
        status = "Initialized" if self._initialized else "Not initialized"
        return f"AgentInterface({status}, brain={'set' if self._brain else 'not set'})"