# NLM Simulation - High-level simulation management

This module provides high-level simulation management utilities for NLM,
making it easier to run complex simulations with proper lifecycle management,
statistics collection, and error handling.
"""

import pynlm
import time
import json
from typing import Dict, Any, List, Optional, Callable
from enum import Enum
from dataclasses import dataclass
from pathlib import Path


class SimulationPhase(Enum):
    """Simulation phases for lifecycle management."""
    SETUP = "setup"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    TERMINATED = "terminated"


@dataclass
class SimulationStats:
    """Container for simulation statistics."""
    steps_completed: int = 0
    total_time_ms: float = 0.0
    steps_per_second: float = 0.0
    average_time_per_step_ms: float = 0.0
    max_neurons_firing: int = 0
    total_spikes: int = 0
    final_development_stage: str = ""
    curiosity_levels: List[float] = None
    novelty_levels: List[float] = None
    
    def __post_init__(self):
        if self.curiosity_levels is None:
            self.curiosity_levels = []
        if self.novelty_levels is None:
            self.novelty_levels = []
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert stats to dictionary."""
        return {
            "steps_completed": self.steps_completed,
            "total_time_ms": self.total_time_ms,
            "steps_per_second": self.steps_per_second,
            "average_time_per_step_ms": self.average_time_per_step_ms,
            "max_neurons_firing": self.max_neurons_firing,
            "total_spikes": self.total_spikes,
            "final_development_stage": self.final_development_stage,
            "curiosity_levels": self.curiosity_levels,
            "novelty_levels": self.novelty_levels,
        }


class Simulation:
    """
    High-level simulation manager for NLM agents.
    
    This class provides a comprehensive simulation management interface with
    automatic resource management, statistics collection, and lifecycle control.
    """
    
    def __init__(self, brain_config=None, world_config=None):
        """
        Initialize a new simulation.
        
        Args:
            brain_config: Configuration for the brain. If None, uses standard config.
            world_config: Configuration for the world. If None, uses default config.
        """
        self.config = brain_config or pynlm.create_standard_config()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.phase = SimulationPhase.SETUP
        self.stats = SimulationStats()
        self.step_callback: Optional[Callable] = None
        self.error_callback: Optional[Callable] = None
        
        self._validate_configuration()
    
    def _validate_configuration(self):
        """Validate simulation configuration."""
        try:
            self.config.validate()
            self.world.validate_configuration()
        except (pynlm.ConfigError, pynlm.WorldError) as e:
            raise pynlm.NLMError(f"Invalid simulation configuration: {e}")
    
    def configure(self, **kwargs):
        """
        Configure world settings.
        
        Args:
            **kwargs: Configuration parameters for the world.
        """
        config_map = {
            "width": "width",
            "height": "height",
            "vision_width": "visionWidth",
            "vision_height": "visionHeight",
            "max_energy": "maxEnergy",
            "energy_decay_rate": "energyDecayRate",
            "random_seed": "randomSeed",
        }
        
        for key, value in kwargs.items():
            if key in config_map:
                setattr(self.world, f"set{config_map[key].title()}({value})")
        
        self._validate_configuration()
    
    def setup(self, agent_start_x=10.0, agent_start_y=10.0):
        """
        Setup the simulation environment.
        
        Args:
            agent_start_x: X coordinate for agent start position.
            agent_start_y: Y coordinate for agent start position.
        """
        if self.phase != SimulationPhase.SETUP:
            raise pynlm.NLMError(f"Cannot setup simulation in phase {self.phase}")
        
        self.brain.initialize()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(agent_start_x, agent_start_y)
        self.agent.initialize(self.world)
        
        self.phase = SimulationPhase.RUNNING
        self.stats = SimulationStats()
    
    def add_step_callback(self, callback: Callable):
        """
        Add a callback function to be called after each step.
        
        Args:
            callback: Function that takes (step, brain, agent, world) arguments.
        """
        self.step_callback = callback
    
    def add_error_callback(self, callback: Callable):
        """
        Add a callback function to be called when errors occur.
        
        Args:
            callback: Function that takes (error) argument.
        """
        self.error_callback = callback
    
    def run(self, num_steps: int, enable_learning=True):
        """
        Run the simulation for a specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run.
            enable_learning: Whether to enable learning features.
        """
        if self.phase != SimulationPhase.RUNNING:
            raise pynlm.NLMError(f"Cannot run simulation in phase {self.phase}")
        
        start_time = time.time()
        
        for step in range(num_steps):
            try:
                # Update world
                self.world.update(0.1)
                
                # Process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Collect statistics
                self.stats.steps_completed += 1
                self.stats.max_neurons_firing = max(
                    self.stats.max_neurons_firing,
                    self.brain.getFiringNeuronCount()
                )
                self.stats.total_spikes += self.brain.getTotalSpikeCount()
                self.stats.curiosity_levels.append(self.agent.getCuriosityLevel())
                self.stats.novelty_levels.append(self.agent.getNoveltyLevel())
                
                # Get action and apply
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply learning
                if enable_learning:
                    self.agent.applyRewardModulation(0.1, 0.0)  # Simple reward
                    self.agent.updateDevelopment(0.1)
                
                # Call step callback if provided
                if self.step_callback:
                    self.step_callback(step, self.brain, self.agent, self.world)
                
            except Exception as e:
                if self.error_callback:
                    self.error_callback(e)
                raise
        
        end_time = time.time()
        self.stats.total_time_ms = (end_time - start_time) * 1000
        self.stats.steps_per_second = (self.stats.steps_completed / 
                                       (self.stats.total_time_ms / 1000.0))
        
        self.phase = SimulationPhase.COMPLETED
    
    def pause(self):
        """Pause the simulation."""
        if self.phase == SimulationPhase.RUNNING:
            self.phase = SimulationPhase.PAUSED
    
    def resume(self):
        """Resume the simulation."""
        if self.phase == SimulationPhase.PAUSED:
            self.phase = SimulationPhase.RUNNING
    
    def terminate(self):
        """Terminate the simulation."""
        self.phase = SimulationPhase.TERMINATED
    
    def get_stats(self) -> SimulationStats:
        """
        Get simulation statistics.
        
        Returns:
            SimulationStats object with simulation results.
        """
        if self.phase == SimulationPhase.RUNNING or self.phase == SimulationPhase.PAUSED:
            if self.stats.steps_completed > 0:
                self.stats.average_time_per_step_ms = (
                    self.stats.total_time_ms / self.stats.steps_completed
                )
                if self.stats.total_time_ms > 0:
                    self.stats.steps_per_second = (
                        self.stats.steps_completed / (self.stats.total_time_ms / 1000.0)
                    )
        
        self.stats.final_development_stage = str(self.brain.getDevelopmentalStage())
        return self.stats
    
    def save(self, filepath: str):
        """
        Save simulation state to file.
        
        Args:
            filepath: Path to save simulation state.
        """
        self.brain.save(filepath)
    
    def load(self, filepath: str):
        """
        Load simulation state from file.
        
        Args:
            filepath: Path to load simulation state from.
        """
        self.brain.load(filepath)
        self._validate_configuration()
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.terminate()


# Make simulation components available at package level
__all__ = [
    "SimulationPhase",
    "SimulationStats",
    "Simulation",
]