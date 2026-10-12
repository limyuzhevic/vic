#!/usr/bin/env python3

"""
NLM Agent - Neural Network Agent Brain

This module provides the AgentBrain class for agents that interact with the world,
combining neural network computation with action selection and decision making.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import numpy as np
import time

from nlm.core.brain import Brain
from nlm.core.world import SimpleWorld
from nlm.core.sensory import Vision, Audio, InternalSignals
from nlm.core.action import Action, MotorCommand

class NLMError(Exception):
    """Base exception for NLM operations."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR", details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = time.time()
    
    def __str__(self):
        return f"[{self.error_code}] {self.message}"

class AgentBrain:
    """Agent brain that interacts with the world."""
    
    def __init__(self, brain_config: Optional[Dict] = None, 
                 world_config: Optional[Dict] = None):
        self.brain_config = brain_config or {}
        self.world_config = world_config or {}
        
        # Initialize components
        self.brain = None
        self.world = None
        self.sensory = {}
        self.action = None
        
        # Agent state
        self.position = {"x": 0, "y": 0}
        self.memory = {}
        self.brain_state = {}
    
    def initialize(self) -> None:
        """Initialize the agent brain components."""
        # Initialize brain
        from nlm.core.brain import Brain
        self.brain = Brain(self.brain_config)
        self.brain.initialize()
        
        # Initialize world
        self.world = SimpleWorld(**self.world_config)
        
        # Initialize sensory systems
        self.sensory["vision"] = Vision()
        self.sensory["audio"] = Audio()
        self.sensory["internal"] = InternalSignals()
        
        # Initialize action system
        from nlm.core.action import MotorCommand
        self.action = MotorCommand()
    
    def run_episode(self, max_steps: int = 1000) -> Dict:
        """
        Run a complete simulation episode.
        
        Args:
            max_steps: Maximum number of steps to run
        
        Returns:
            Episode statistics
        """
        episode_stats = {
            "total_reward": 0.0,
            "steps": 0,
            "success": False,
            "brain_metrics": {}
        }
        
        for step in range(max_steps):
            # Get observation from sensory systems
            observation = self._get_observation()
            
            # Select action based on brain state
            action = self._select_action(observation)
            
            # Execute action in world
            world_result = self.world.step(action)
            
            # Update brain with world state
            self._update_brain(observation, action, world_result)
            
            # Update agent state
            self._update_state(action, world_result)
            
            # Accumulate rewards
            episode_stats["total_reward"] += world_result["reward"]
            episode_stats["steps"] = step + 1
            
            # Check for termination
            if world_result["done"]:
                episode_stats["success"] = True
                break
            
            # Store brain metrics periodically
            if step % 100 == 0:
                episode_stats["brain_metrics"][step] = self.brain.compute_metrics()
        
        return episode_stats
    
    def _get_observation(self) -> Dict:
        """Get current observation from sensory systems."""
        obs = {}
        
        # Vision observation
        obs["vision"] = self.sensory["vision"].get_observation()
        
        # Audio observation
        obs["audio"] = self.sensory["audio"].get_observation()
        
        # Internal signals observation
        obs["internal"] = self.sensory["internal"].get_observation()
        
        # World state
        obs["world"] = self.world.get_observation()
        
        # Agent state
        obs["agent"] = {
            "position": self.position,
            "memory": self.memory,
            "brain_state": self.brain_state
        }
        
        return obs
    
    def _select_action(self, observation: Dict) -> Dict:
        """Select action based on observation."""
        # Convert observation to brain input
        brain_input = self._prepare_brain_input(observation)
        
        # Update brain state
        self.brain_state = self.brain.process_input(brain_input)
        
        # Select action based on brain output
        action_type = self.brain_state.get("action", "wait")
        
        # Convert action type to motor command
        action = self.action.create_motor_command(action_type)
        
        return action
    
    def _update_brain(self, observation: Dict, action: Dict, 
                      world_result: Dict) -> None:
        """Update brain state with new information."""
        # Create learning signal from reward
        reward = world_result["reward"]
        
        # Process experience
        experience = {
            "observation": observation,
            "action": action,
            "reward": reward,
            "next_observation": world_result["observation"],
            "done": world_result["done"]
        }
        
        self.brain.learn_from_experience(experience)
    
    def _update_state(self, action: Dict, world_result: Dict) -> None:
        """Update agent state after action execution."""
        # Update position from action
        if "move" in action:
            self.position["x"] += action["move"][0]
            self.position["y"] += action["move"][1]
        
        # Update memory with recent experience
        self.memory["last_action"] = action
        self.memory["last_reward"] = world_result["reward"]
        self.memory["last_observation"] = world_result["observation"]
        self.memory["timestamp"] = time.time()
    
    def _prepare_brain_input(self, observation: Dict) -> Dict:
        """Prepare brain input from observation."""
        brain_input = {}
        
        # Extract relevant features from sensory observations
        for sensor_name, sensor_obs in observation.items():
            if isinstance(sensor_obs, dict):
                brain_input[sensor_name] = sensor_obs
        
        return brain_input
    
    def close(self) -> None:
        """Clean up agent brain resources."""
        if self.world:
            self.world.close()
        
        self.brain = None
        self.world = None
        self.sensory = {}
        self.action = None

class NLMContextManager:
    """Context manager for NLM resources."""
    
    def __init__(self, auto_save: bool = True, auto_profiling: bool = False):
        self.auto_save = auto_save
        self.auto_profiling = auto_profiling
        self.session_id = id(self)
        self.start_time = None
        self.brain = None
        self.experiment = None
        self.monitor = None
    
    def __enter__(self):
        self.start_time = time.time()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        duration = time.time() - self.start_time if self.start_time else 0
        
        if exc_type:
            print(f"NLM Session {self.session_id} ended with error: {exc_val}")
        else:
            print(f"NLM Session {self.session_id} completed successfully in {duration:.2f}s")
        
        if self.auto_save and self.brain:
            self.save_session()
    
    def create_brain(self, config: 'Config') -> 'Brain':
        """Create a brain for this session."""
        from nlm.core.brain import Brain
        self.brain = Brain(config)
        self.brain.initialize()
        return self.brain
    
    def create_monitor(self) -> 'RealTimeMonitor':
        """Create a monitor for this session."""
        from nlm.monitoring.real_time import RealTimeMonitor
        self.monitor = RealTimeMonitor()
        self.monitor.start()
        return self.monitor
    
    def save_session(self, prefix: str = "session") -> tuple:
        """Save current session state."""
        timestamp = time.time()
        brain_file = f"{prefix}_{self.session_id}_brain.pkl"
        
        if self.brain:
            from nlm.core.brain import save_brain
            save_brain(self.brain, brain_file)
        
        return brain_file
    
    def load_session(self, brain_file: str) -> 'Brain':
        """Load session state."""
        from nlm.core.brain import load_brain
        if brain_file:
            self.brain = load_brain(brain_file)
        
        return self.brain

# Import types
import time
from nlm.core.brain import Brain, save_brain, load_brain
from nlm.core.config import Config
from nlm.core.action import MotorCommand

__all__ = [
    "AgentBrain",
    "NLMError",
    "NLMContextManager",
    "__version__"
]