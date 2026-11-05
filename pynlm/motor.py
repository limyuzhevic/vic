"""
Motor system classes for NLM
"""

from typing import List, Dict, Any
from enum import Enum
from dataclasses import dataclass
from .types import ActionType, MotorCommand, Timestamp, Action, ActionResult

@dataclass
class Action(Action):
    """Action representation for motor output"""
    
    def __init__(self, action_type: ActionType = ActionType.Wait, parameters: List[float] = None):
        super().__init__()
        self.type = action_type
        self.parameters = parameters or []
        self.timestamp = Timestamp(time.time())
    
    def getType(self) -> ActionType:
        """Get the action type"""
        return self.type
    
    def setType(self, action_type: ActionType) -> None:
        """Set the action type"""
        self.type = action_type
    
    def getParameters(self) -> List[float]:
        """Get action parameters"""
        return self.parameters
    
    def setParameters(self, parameters: List[float]) -> None:
        """Set action parameters"""
        self.parameters = parameters
    
    def getName(self) -> str:
        """Get human-readable action name"""
        return self.type.name.replace('_', ' ').title()
    
    def clone(self) -> 'Action':
        """Create a copy of this action"""
        return Action(self.type, self.parameters.copy())
    
    def __repr__(self) -> str:
        params_str = f"[{', '.join(f'{p:.3f}' for p in self.parameters)}]" if self.parameters else "[]"
        return f"<Action: {self.getName()}{params_str}>"


@dataclass
class ActionResult(ActionResult):
    """Action result from world interaction"""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        super().__init__()
        self.reward = reward
        self.success = success
        self.message = message
        self.timestamp = Timestamp(time.time())
    
    def getReward(self) -> float:
        """Get reward value"""
        return self.reward
    
    def getSuccess(self) -> bool:
        """Get success flag"""
        return self.success
    
    def getMessage(self) -> str:
        """Get result message"""
        return self.message
    
    def __repr__(self) -> str:
        status = "SUCCESS" if self.success else "FAILURE"
        return f"<ActionResult: {status} (+{self.reward:.3f}) - {self.message}>"


# Helper functions for common actions
def create_move_action(direction: str, intensity: float = 1.0) -> Action:
    """Create a movement action"""
    direction_map = {
        'forward': ActionType.MoveForward,
        'backward': ActionType.MoveBackward,
        'left': ActionType.MoveLeft,
        'right': ActionType.MoveRight,
        'up': ActionType.LookUp,
        'down': ActionType.LookDown,
        'turn_left': ActionType.TurnLeft,
        'turn_right': ActionType.TurnRight,
    }
    
    action_type = direction_map.get(direction.lower(), ActionType.Wait)
    return Action(action_type, [intensity])

def create_interaction_action(interaction_type: str, strength: float = 1.0) -> Action:
    """Create an interaction action"""
    interaction_map = {
        'look': ActionType.Look,
        'interact': ActionType.Interact,
        'eat': ActionType.Eat,
        'drink': ActionType.Drink,
        'rest': ActionType.Rest,
        'wait': ActionType.Wait,
    }
    
    action_type = interaction_map.get(interaction_type.lower(), ActionType.Wait)
    return Action(action_type, [strength])


def parse_action_from_string(action_str: str) -> Action:
    """Parse an action from a string description"""
    # Simple parser for action strings
    action_str = action_str.lower().strip()
    
    if action_str in ['move forward', 'moveforward']:
        return create_move_action('forward')
    elif action_str in ['move backward', 'movebackward']:
        return create_move_action('backward')
    elif action_str in ['move left', 'moveleft']:
        return create_move_action('left')
    elif action_str in ['move right', 'moveright']:
        return create_move_action('right')
    elif action_str in ['look up', 'lookup']:
        return create_move_action('up')
    elif action_str in ['look down', 'lookdown']:
        return create_move_action('down')
    elif action_str in ['turn left', 'turnleft']:
        return create_move_action('turn_left')
    elif action_str in ['turn right', 'turnright']:
        return create_move_action('turn_right')
    elif action_str in ['look', 'look_around']:
        return create_interaction_action('look')
    elif action_str in ['interact', 'interact_with']:
        return create_interaction_action('interact')
    elif action_str in ['eat', 'consume']:
        return create_interaction_action('eat')
    elif action_str in ['drink', 'hydrate']:
        return create_interaction_action('drink')
    elif action_str in ['rest', 'sleep']:
        return create_interaction_action('rest')
    elif action_str in ['wait', 'stay']:
        return create_interaction_action('wait')
    else:
        # Default to wait action for unrecognized commands
        return Action(ActionType.Wait)
