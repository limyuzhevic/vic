#!/usr/bin/env python3

"""
NLM World - Neural Network World Simulation

This module provides the World class for simulating neural network environments,
including sensory inputs, actions, and interactions.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import numpy as np

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

class WorldError(NLMError):
    """World simulation error."""
    
    def __init__(self, message: str, world_type: Optional[str] = None):
        super().__init__(message, "WORLD_ERROR", {
            "world_type": world_type,
            "timestamp": time.time()
        })

class SimpleWorld:
    """Simple world simulation environment."""
    
    def __init__(self, width: int = 100, height: int = 100, 
                 obstacles: Optional[List[Dict]] = None):
        self.width = width
        self.height = height
        self.obstacles = obstacles or []
        self.agent_position = {"x": 0, "y": 0}
        self.objects = []
        self.timestamp = 0
    
    def get_observation(self) -> Dict:
        """Get current world observation."""
        return {
            "agent_position": self.agent_position,
            "width": self.width,
            "height": self.height,
            "obstacles": self.obstacles,
            "objects": self.objects,
            "timestamp": self.timestamp
        }
    
    def step(self, action: Dict) -> Dict:
        """Execute one simulation step."""
        self.timestamp += 1
        
        # Process action
        if "move" in action:
            dx, dy = action["move"]
            new_x = self.agent_position["x"] + dx
            new_y = self.agent_position["y"] + dy
            
            # Check bounds
            new_x = max(0, min(self.width - 1, new_x))
            new_y = max(0, min(self.height - 1, new_y))
            
            self.agent_position = {"x": new_x, "y": new_y}
        
        # Generate reward based on position
        reward = 0.0
        for obstacle in self.obstacles:
            if (self.agent_position["x"] == obstacle["x"] and 
                self.agent_position["y"] == obstacle["y"]):
                reward -= 10.0
        
        # Check for goal
        if (self.agent_position["x"] == self.width - 1 and 
            self.agent_position["y"] == self.height - 1):
            reward += 100.0
        
        return {
            "observation": self.get_observation(),
            "reward": reward,
            "done": False,
            "info": {"step": self.timestamp}
        }
    
    def reset(self) -> Dict:
        """Reset the world."""
        self.agent_position = {"x": 0, "y": 0}
        self.timestamp = 0
        return self.get_observation()
    
    def close(self) -> None:
        """Clean up world resources."""
        pass

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

__all__ = [
    "SimpleWorld",
    "NLMError",
    "WorldError",
    "NLMContextManager",
    "__version__"
]