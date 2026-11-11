# Motor module - Enhanced Python interface for motor actions
"""
Enhanced Python interface for Action class with additional functionality
for motor command generation and control.
"""

from typing import List, Optional, Any, Dict, Union
import numpy as np

# Try to import C++ bindings
try:
    from ._pynlm import Action as _Action
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False

class Action:
    """Enhanced Python interface for Action class.
    
    This class provides a more Pythonic interface for motor actions,
    with enhanced convenience methods, parameterization, and action
    composition capabilities.
    """
    
    def __init__(self, action_type=None, parameters=None):
        """Initialize action.
        
        Args:
            action_type: Action type (optional).
            parameters: Action parameters (optional).
        """
        if CPLUSPLUS_AVAILABLE and _Action is not None:
            self._action = _Action(action_type)
        else:
            # Create mock action for testing
            self._action = MockAction(action_type)
        
        self._parameters = parameters or {}
    
    def get_type(self):
        """Get action type.
        
        Returns:
            ActionType enum value.
        """
        return self._action.getType()
    
    def set_type(self, action_type):
        """Set action type.
        
        Args:
            action_type: New action type.
        """
        return self._action.setType(action_type)
    
    def get_parameters(self):
        """Get action parameters.
        
        Returns:
            Dictionary of action parameters.
        """
        params = self._parameters.copy()
        # Get C++ parameters too
        if hasattr(self._action, 'getParameters'):
            cpp_params = self._action.getParameters()
            params.update(cpp_params)
        return params
    
    def set_parameters(self, parameters):
        """Set action parameters.
        
        Args:
            parameters: New parameters dictionary.
        """
        self._parameters.update(parameters)
        if hasattr(self._action, 'setParameters'):
            return self._action.setParameters(parameters)
    
    def get_name(self):
        """Get action name.
        
        Returns:
            Action name string.
        """
        return self._action.getName()
    
    def clone(self):
        """Create a copy of this action.
        
        Returns:
            New Action instance.
        """
        new_action = Action()
        new_action.set_type(self.get_type())
        new_action.set_parameters(self.get_parameters())
        return new_action
    
    def apply_modification(self, modification):
        """Apply modification to action parameters.
        
        Args:
            modification: Modification dictionary or function.
        """
        if isinstance(modification, dict):
            for key, value in modification.items():
                self._parameters[key] = value
        elif callable(modification):
            self._parameters = modification(self._parameters)
        
        if hasattr(self._action, 'setParameters'):
            return self._action.setParameters(self._parameters)
    
    # Action-specific methods
    
    def is_movement(self):
        """Check if action is a movement action.
        
        Returns:
            True if action is movement, False otherwise.
        """
        from .types import ActionType
        movement_types = {
            ActionType.MoveForward, ActionType.MoveBackward,
            ActionType.MoveLeft, ActionType.MoveRight,
            ActionType.TurnLeft, ActionType.TurnRight,
        }
        return self.get_type() in movement_types
    
    def is_look(self):
        """Check if action is a look action.
        
        Returns:
            True if action is look, False otherwise.
        """
        from .types import ActionType
        return self.get_type() in {ActionType.Look, ActionType.LookUp, ActionType.LookDown}
    
    def is_interaction(self):
        """Check if action is an interaction action.
        
        Returns:
            True if action is interaction, False otherwise.
        """
        from .types import ActionType
        return self.get_type() in {ActionType.Interact, ActionType.Eat, ActionType.Drink}
    
    def get_target_position(self):
        """Get target position if action has one.
        
        Returns:
            Tuple (x, y) or None if no target position.
        """
        return self._parameters.get('target_position')
    
    def set_target_position(self, x, y):
        """Set target position for movement actions.
        
        Args:
            x: Target X coordinate.
            y: Target Y coordinate.
        """
        self._parameters['target_position'] = (x, y)
        if hasattr(self._action, 'setParameters'):
            return self._action.setParameters(self._parameters)
    
    def get_duration(self):
        """Get action duration.
        
        Returns:
            Duration in seconds.
        """
        return self._parameters.get('duration', 1.0)
    
    def set_duration(self, duration):
        """Set action duration.
        
        Args:
            duration: Duration in seconds.
        """
        self._parameters['duration'] = duration
        if hasattr(self._action, 'setParameters'):
            return self._action.setParameters(self._parameters)
    
    def get_intensity(self):
        """Get action intensity.
        
        Returns:
            Intensity value (0-1).
        """
        return self._parameters.get('intensity', 1.0)
    
    def set_intensity(self, intensity):
        """Set action intensity.
        
        Args:
            intensity: Intensity value (0-1).
        """
        self._parameters['intensity'] = max(0.0, min(1.0, intensity))
        if hasattr(self._action, 'setParameters'):
            return self._action.setParameters(self._parameters)
    
    def get_velocity(self):
        """Get action velocity.
        
        Returns:
            Velocity value.
        """
        return self._parameters.get('velocity', 1.0)
    
    def set_velocity(self, velocity):
        """Set action velocity.
        
        Args:
            velocity: Velocity value.
        """
        self._parameters['velocity'] = velocity
        if hasattr(self._action, 'setParameters'):
            return self._action.setParameters(self._parameters)
    
    def to_dict(self):
        """Convert action to dictionary.
        
        Returns:
            Dictionary representation.
        """
        return {
            'type': str(self.get_type()),
            'parameters': self.get_parameters(),
            'name': self.get_name(),
        }
    
    def from_dict(self, data):
        """Create action from dictionary.
        
        Args:
            data: Dictionary with action data.
        """
        self.set_type(data['type'])
        self.set_parameters(data.get('parameters', {}))
        return self
    
    def __repr__(self):
        """String representation."""
        params_str = ", ".join(f"{k}={v}" for k, v in self._parameters.items())
        return f"<Action: {self.get_type()}({params_str})>"
    
    def __str__(self):
        """Human-readable string."""
        return f"{self.get_name()} with {self.get_parameters()}"


# Mock action class for testing when C++ bindings are not available
class MockAction:
    def __init__(self, action_type=None):
        from .types import ActionType
        self.action_type = action_type or ActionType.Wait
        
    def getType(self):
        return self.action_type
        
    def setType(self, action_type):
        self.action_type = action_type
        return True
        
    def getName(self):
        from .types import ActionType
        names = {
            ActionType.MoveForward: "Move Forward",
            ActionType.MoveBackward: "Move Backward",
            ActionType.MoveLeft: "Move Left",
            ActionType.MoveRight: "Move Right",
            ActionType.TurnLeft: "Turn Left",
            ActionType.TurnRight: "Turn Right",
            ActionType.Look: "Look",
            ActionType.LookUp: "Look Up",
            ActionType.LookDown: "Look Down",
            ActionType.Interact: "Interact",
            ActionType.Eat: "Eat",
            ActionType.Drink: "Drink",
            ActionType.Rest: "Rest",
            ActionType.Wait: "Wait",
            ActionType.Custom: "Custom",
        }
        return names.get(self.action_type, "Unknown")
        
    def getParameters(self):
        return {}
        
    def setParameters(self, parameters):
        return True


# Action factory and convenience functions

def create_action(action_type=None):
    """Create action with optional type.
    
    Args:
        action_type: Action type (optional).
        
    Returns:
        Action instance.
    """
    return Action(action_type)


def create_movement_action(direction, speed=1.0, duration=1.0):
    """Create movement action.
    
    Args:
        direction: Movement direction (string).
        speed: Movement speed (0-1).
        duration: Action duration.
        
    Returns:
        Movement Action instance.
    """
    from .types import ActionType
    
    direction_map = {
        'forward': ActionType.MoveForward,
        'backward': ActionType.MoveBackward,
        'left': ActionType.MoveLeft,
        'right': ActionType.MoveRight,
        'turn_left': ActionType.TurnLeft,
        'turn_right': ActionType.TurnRight,
    }
    
    action_type = direction_map.get(direction.lower(), ActionType.Wait)
    action = Action(action_type)
    action.set_velocity(speed)
    action.set_duration(duration)
    return action


def create_look_action(direction, intensity=1.0):
    """Create look action.
    
    Args:
        direction: Look direction (string).
        intensity: Look intensity (0-1).
        
    Returns:
        Look Action instance.
    """
    from .types import ActionType
    
    direction_map = {
        'up': ActionType.LookUp,
        'down': ActionType.LookDown,
        'left': ActionType.LookLeft,
        'right': ActionType.LookRight,
    }
    
    action_type = direction_map.get(direction.lower(), ActionType.Look)
    action = Action(action_type)
    action.set_intensity(intensity)
    return action


def create_interaction_action(action_type, parameters=None):
    """Create interaction action.
    
    Args:
        action_type: Interaction type (string).
        parameters: Additional parameters (optional).
        
    Returns:
        Interaction Action instance.
    """
    from .types import ActionType
    
    action_map = {
        'interact': ActionType.Interact,
        'eat': ActionType.Eat,
        'drink': ActionType.Drink,
        'rest': ActionType.Rest,
        'wait': ActionType.Wait,
        'custom': ActionType.Custom,
    }
    
    action_type_enum = action_map.get(action_type.lower(), ActionType.Custom)
    action = Action(action_type_enum)
    if parameters:
        action.set_parameters(parameters)
    return action


def compose_actions(*actions, blend_factor=0.5):
    """Compose multiple actions.
    
    Args:
        *actions: Actions to compose.
        blend_factor: Blend factor for parameters (0-1).
        
    Returns:
        Composed action.
    """
    if not actions:
        return create_action()
    
    # Start with first action
    composed = actions[0].clone()
    
    # Blend parameters from other actions
    for action in actions[1:]:
        for param, value in action.get_parameters().items():
            if param in composed.get_parameters():
                # Blend numeric parameters
                try:
                    current = composed.get_parameters()[param]
                    if isinstance(current, (int, float)):
                        blended = (current * (1 - blend_factor) + 
                                 value * blend_factor)
                        composed.set_parameters({param: blended})
                except (TypeError, ValueError):
                    # Don't blend non-numeric parameters
                    pass
    
    return composed


# Predefined action sequences

def get_walking_sequence(speed=0.5, duration=1.0):
    """Get a simple walking sequence.
    
    Args:
        speed: Walking speed.
        duration: Duration per step.
        
    Returns:
        List of movement actions.
    """
    return [
        create_movement_action('forward', speed, duration),
        create_movement_action('right', speed, duration),
        create_movement_action('forward', speed, duration),
        create_movement_action('left', speed, duration),
    ]


def get_explore_sequence(speed=0.3, duration=0.5):
    """Get exploration sequence (random movements).
    
    Args:
        speed: Movement speed.
        duration: Duration per step.
        
    Returns:
        List of exploration actions.
    """
    import random
    
    directions = ['forward', 'backward', 'left', 'right']
    sequence = []
    
    for _ in range(8):
        direction = random.choice(directions)
        action = create_movement_action(direction, speed, duration)
        sequence.append(action)
    
    return sequence


# Re-export for convenience
Action = Action
create_action = create_action
create_movement_action = create_movement_action
create_look_action = create_look_action
create_interaction_action = create_interaction_action
compose_actions = compose_actions
get_walking_sequence = get_walking_sequence
get_explore_sequence = get_explore_sequence