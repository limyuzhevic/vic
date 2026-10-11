"""
Agent wrapper for NLM brain-world interface

This module provides a Pythonic interface to the C++ AgentBrain class with
convenience methods for agent-based simulation and behavior control.
"""

from typing import Optional, List, Any, Dict
from enum import Enum

# Import the C++ bindings
from .pynlm import (
    AgentBrain as CppAgentBrain,
    createAgentBrain as create_cpp_agent_brain,
    Brain as CppBrain,
    SimpleWorld as CppSimpleWorld,
    SensoryPercept,
    Vision,
    Audio,
    InternalSignals,
    Action,
    MotorCommand,
    ActionType,
    Reward,
    PredictionError,
    Novelty,
    Neuromodulator,
    DevelopmentalStage,
)

class AgentBrain:
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain: Optional[CppBrain] = None):
        """Create an agent brain interface.
        
        Args:
            brain: Optional underlying brain instance
        """
        if brain is None:
            from .brain import create_brain
            brain = create_brain()
        
        self._cpp_agent = create_cpp_agent_brain(brain)
    
    def initialize(self, world: SimpleWorld):
        """Initialize with world.
        
        Args:
            world: World instance to initialize with
        """
        self._cpp_agent.initialize(world._cpp_world)
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            Expected sensory input size
        """
        return self._cpp_agent.getSensoryInputSize()
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size.
        
        Returns:
            Expected motor output size
        """
        return self._cpp_agent.getMotorOutputSize()
    
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process
        """
        self._cpp_agent.processSensoryInput(percept)
    
    def decode_motor_command(self) -> Action:
        """Decode brain motor activity into motor command.
        
        Returns:
            Motor command
        """
        return self._cpp_agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Expected/predicted reward
        """
        self._cpp_agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float):
        """Update development system.
        
        Args:
            timestep: Time step duration
        """
        self._cpp_agent.updateDevelopment(timestep)
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage
        """
        return self._cpp_agent.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Neuromodulation level (0.0-1.0)
        """
        return self._cpp_agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level.
        
        Returns:
            Curiosity level (0.0-1.0)
        """
        return self._cpp_agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level.
        
        Returns:
            Novelty level (0.0-1.0)
        """
        return self._cpp_agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> PredictionError:
        """Get prediction error.
        
        Returns:
            Prediction error
        """
        return self._cpp_agent.getPredictionError()
    
    def reset(self):
        """Reset agent for new episode."""
        self._cpp_agent.reset()
    
    def get_brain(self) -> Brain:
        """Get the underlying brain.
        
        Returns:
            Underlying brain instance
        """
        from .brain import Brain
        return Brain(self._cpp_agent.getBrain())
    
    def enable_reward_modulation(self, enable: bool):
        """Enable/disable reward modulation.
        
        Args:
            enable: Whether to enable reward modulation
        """
        self._cpp_agent.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool):
        """Enable/disable structural plasticity.
        
        Args:
            enable: Whether to enable structural plasticity
        """
        self._cpp_agent.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool):
        """Enable/disable development.
        
        Args:
            enable: Whether to enable development
        """
        self._cpp_agent.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool):
        """Enable/disable curiosity.
        
        Args:
            enable: Whether to enable curiosity
        """
        self._cpp_agent.enableCuriosity(enable)
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled
        """
        return self._cpp_agent.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled
        """
        return self._cpp_agent.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            True if development is enabled
        """
        return self._cpp_agent.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            True if curiosity is enabled
        """
        return self._cpp_agent.isCuriosityEnabled()
    
    def get_neuromodulation_stats(self) -> Dict[str, Any]:
        """Get comprehensive neuromodulation statistics.
        
        Returns:
            Dictionary with neuromodulation statistics
        """
        return {
            'reward_modulation_enabled': self.is_reward_modulation_enabled(),
            'structural_plasticity_enabled': self.is_structural_plasticity_enabled(),
            'development_enabled': self.is_development_enabled(),
            'curiosity_enabled': self.is_curiosity_enabled(),
            'neuromodulation_level': self.get_neuromodulation_level(),
            'curiosity_level': self.get_curiosity_level(),
            'novelty_level': self.get_novelty_level(),
        }
    
    def enable_learning_features(self, reward_mod: bool = True, curiosity: bool = True,
                                structural: bool = False, development: bool = False):
        """Enable multiple learning features at once.
        
        Args:
            reward_mod: Enable reward modulation
            curiosity: Enable curiosity
            structural: Enable structural plasticity
            development: Enable development
        """
        self.enable_reward_modulation(reward_mod)
        self.enable_curiosity(curiosity)
        self.enable_structural_plasticity(structural)
        self.enable_development(development)
    
    def run_episode(self, world: SimpleWorld, steps: int, 
                   timestep: float = 0.1, 
                   progress_callback: Optional[callable] = None,
                   render_callback: Optional[callable] = None):
        """Run a complete simulation episode.
        
        Args:
            world: World instance
            steps: Number of simulation steps
            timestep: Time step duration
            progress_callback: Optional progress callback function
            render_callback: Optional render callback function
        """
        world.reset()
        self.reset()
        
        for step in range(steps):
            # Update world
            world.update(timestep)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            self.process_sensory_input(percept)
            
            # Update brain
            brain = self.get_brain()
            brain.step(step)
            
            # Get action
            action = self.decode_motor_command()
            
            # Apply action
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Update development
            self.update_development(timestep)
            
            # Call progress callback
            if progress_callback:
                progress = (step / steps) * 100
                stats = {
                    'step': step,
                    'progress': progress,
                    'percept': percept,
                    'action': action,
                }
                progress_callback(stats)
            
            # Call render callback
            if render_callback:
                render_callback(world, percept, action, brain.get_stats())
    
    def __repr__(self) -> str:
        """Get string representation of agent.
        
        Returns:
            String representation
        """
        stats = self.get_neuromodulation_stats()
        stage = self.get_developmental_stage()
        return (f"AgentBrain(stage={stage}, "
                f"reward_mod={stats['reward_modulation_enabled']}, "
                f"curiosity={stats['curiosity_enabled']})")

# Convenience functions

def create_agent_brain(brain: Optional[Brain] = None) -> AgentBrain:
    """Create an agent brain interface.
    
    Args:
        brain: Optional brain instance
        
    Returns:
        AgentBrain instance
    """
    return AgentBrain(brain)

# Export types and classes
__all__ = [
    'AgentBrain',
    'create_agent_brain',
    'SensoryPercept',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'MotorCommand',
    'ActionType',
    'Reward',
    'PredictionError',
    'Novelty',
    'Neuromodulator',
    'DevelopmentalStage',
]