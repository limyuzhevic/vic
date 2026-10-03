# -*- coding: utf-8 -*-
"""
High-level Agent abstraction for NLM Python package.

This module provides a high-level interface for working with NLM agents,
abstracting away some of the low-level details and providing more Pythonic APIs.
"""

from typing import Optional, List, Dict, Any

from .bindings import (
    AgentBrain as _AgentBrain,
    Brain as _Brain,
    SimpleWorld as _SimpleWorld,
    SensoryPercept as _SensoryPercept,
    MotorCommand,
    ActionType,
    Action as _Action,
    SimulationStep,
    Timestamp,
    DevelopmentalStage,
)
class Agent:
    """
    High-level interface for NLM agent simulation.
    
    This class wraps the low-level C++ AgentBrain class and provides a more
    Pythonic interface for common operations.
    """
    
    def __init__(self, brain: _Brain):
        """
        Initialize a new agent.
        
        Args:
            brain: Brain instance to control
        """
        self._agent = _AgentBrain(brain)
        self._brain = brain
        self._world = None
    
    def initialize(self, world: _SimpleWorld):
        """
        Initialize the agent with a world.
        
        Args:
            world: World instance to interact with
        """
        self._agent.initialize(world)
        self._world = world
        return self
    
    def get_sensory_input_size(self) -> int:
        """
        Get expected sensory input size.
        
        Returns:
            Sensory input size
        """
        return self._agent.getSensoryInputSize()
    
    def get_motor_output_size(self) -> int:
        """
        Get expected motor output size.
        
        Returns:
            Motor output size
        """
        return self._agent.getMotorOutputSize()
    
    def process_sensory_input(self, percept: _SensoryPercept):
        """
        Process sensory input from the world.
        
        Args:
            percept: Sensory percept from world
        """
        self._agent.processSensoryInput(percept)
        return self
    
    def decode_motor_command(self) -> MotorCommand:
        """
        Decode motor command from brain.
        
        Returns:
            Motor command
        """
        return self._agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        self._agent.applyRewardModulation(reward, predicted_reward)
        return self
    
    def update_development(self, timestep: float):
        """
        Update development system.
        
        Args:
            timestep: Time step duration
        """
        self._agent.updateDevelopment(timestep)
        return self
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """
        Get current developmental stage.
        
        Returns:
            Developmental stage
        """
        return self._agent.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """
        Get current neuromodulation level.
        
        Returns:
            Neuromodulation level
        """
        return self._agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """
        Get curiosity level.
        
        Returns:
            Curiosity level
        """
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """
        Get novelty level.
        
        Returns:
            Novelty level
        """
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """
        Get prediction error.
        
        Returns:
            Prediction error
        """
        return self._agent.getPredictionError()
    
    def reset(self):
        """Reset agent for new episode."""
        self._agent.reset()
        return self
    
    def enable_reward_modulation(self, enable: bool):
        """
        Enable or disable reward modulation.
        
        Args:
            enable: Whether to enable reward modulation
        """
        self._agent.enableRewardModulation(enable)
        return self
    
    def enable_structural_plasticity(self, enable: bool):
        """
        Enable or disable structural plasticity.
        
        Args:
            enable: Whether to enable structural plasticity
        """
        self._agent.enableStructuralPlasticity(enable)
        return self
    
    def enable_development(self, enable: bool):
        """
        Enable or disable development.
        
        Args:
            enable: Whether to enable development
        """
        self._agent.enableDevelopment(enable)
        return self
    
    def enable_curiosity(self, enable: bool):
        """
        Enable or disable curiosity.
        
        Args:
            enable: Whether to enable curiosity
        """
        self._agent.enableCuriosity(enable)
        return self
    
    def is_reward_modulation_enabled(self) -> bool:
        """
        Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled
        """
        return self._agent.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """
        Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled
        """
        return self._agent.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """
        Check if development is enabled.
        
        Returns:
            True if development is enabled
        """
        return self._agent.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """
        Check if curiosity is enabled.
        
        Returns:
            True if curiosity is enabled
        """
        return self._agent.isCuriosityEnabled()
    
    def get_brain(self) -> _Brain:
        """
        Get the underlying brain.
        
        Returns:
            Brain instance
        """
        return self._agent.getBrain()
    
    def run_simulation(self, world: _SimpleWorld, num_steps: int,
                      timestep: float = 0.1, verbose: bool = False):
        """
        Run a complete simulation.
        
        Args:
            world: World instance
            num_steps: Number of simulation steps
            timestep: Time step duration
            verbose: Whether to print progress
        """
        self.initialize(world)
        
        for step in range(num_steps):
            world.update(timestep)
            self.process_sensory_input(world.getSensoryPercept())
            self.get_brain().step(step)
            action = self.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
            
            if verbose and step % 10 == 0:
                stats = self.get_brain().get_stats()
                print(f"Step {step}: "
                      f"{stats['firing_neurons']} neurons firing, "
                      f"{stats['total_spikes']} total spikes")
        
        return self
    
    def __repr__(self):
        return f"<Agent connected to {self._brain}>"


def createAgentBrain(brain: _Brain) -> Agent:
    """
    Create a new agent brain interface.
    
    Args:
        brain: Brain instance to control
    
    Returns:
        New Agent instance
    """
    return Agent(brain)


# For backward compatibility
Agent = createAgentBrain
