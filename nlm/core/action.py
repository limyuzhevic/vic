#!/usr/bin/env python3

"""
NLM Action - Neural Network Action Systems

This module provides action system classes for agents to interact with their environment:
- Action: Base action class
- MotorCommand: Low-level motor control
"""

from typing import Dict, List, Optional, Any, Union, Enum
import numpy as np
import time

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

class ActionType(Enum):
    """Enumeration of action types."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"

class MotorCommand:
    """Low-level motor command."""
    
    def __init__(self, command_type: ActionType = ActionType.WAIT,
                 parameters: Optional[Dict] = None):
        self.command_type = command_type
        self.parameters = parameters or {}
        self.duration = 0.0
        self.speed = 1.0
        self.accuracy = 1.0
        self.timestamp = time.time()
    
    def get_parameters(self, param_name: str, default_value: Any = None) -> Any:
        """Get a parameter value."""
        return self.parameters.get(param_name, default_value)
    
    def set_parameters(self, parameters: Dict) -> None:
        """Set parameters."""
        self.parameters.update(parameters)
    
    def execute(self) -> Dict:
        """Execute the motor command."""
        result = {
            "command_type": self.command_type,
            "parameters": self.parameters,
            "duration": self.duration,
            "speed": self.speed,
            "accuracy": self.accuracy,
            "timestamp": self.timestamp,
            "executed": True
        }
        
        return result

class Action:
    """Base action class."""
    
    def __init__(self, action_type: ActionType, target: Optional[Dict] = None,
                 preconditions: Optional[List[Callable]] = None,
                 effects: Optional[List[Callable]] = None):
        self.action_type = action_type
        self.target = target or {}
        self.preconditions = preconditions or []
        self.effects = effects or []
        self.required_resources = []
        self.cost = 0.0
        self.priority = 1.0
        self.timestamp = time.time()
    
    def can_execute(self, context: Dict) -> bool:
        """Check if action can be executed given context."""
        # Check preconditions
        for precondition in self.preconditions:
            try:
                if not precondition(context):
                    return False
            except Exception:
                return False
        
        # Check required resources
        for resource in self.required_resources:
            if resource not in context.get("resources", {}):
                return False
        
        return True
    
    def execute(self, context: Dict) -> Dict:
        """Execute the action."""
        if not self.can_execute(context):
            raise NLMError(f"Cannot execute action {self.action_type}", "ACTION_ERROR")
        
        # Execute action
        result = {
            "action_type": self.action_type,
            "target": self.target,
            "success": True,
            "new_context": context.copy(),
            "timestamp": time.time()
        }
        
        # Apply effects
        for effect in self.effects:
            try:
                effect(result["new_context"])
            except Exception as e:
                raise NLMError(f"Error applying effect: {e}", "ACTION_EFFECT_ERROR")
        
        return result

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
    "ActionType",
    "MotorCommand",
    "Action",
    "NLMError",
    "NLMContextManager",
    "__version__"
]