"""
Motor control and action execution.

This module contains motor control components and advanced action
execution features for expert users.
"""

from .core import *

class MotorController:
    """
    Advanced motor controller for expert users.
    
    Provides advanced motor control, action planning, and
    motor learning capabilities.
    """
    
    def __init__(self):
        self._actions = []
        self._action_planner = None
        self._motor_learner = None
        self._motor_properties = {}
        self._last_action = None
        self._action_history = []
        
    def add_action(self, action):
        """Add an action to the controller."""
        self._actions.append(action)
        
    def set_action_planner(self, planner):
        """Set action planning function."""
        self._action_planner = planner
        
    def set_motor_learner(self, learner):
        """Set motor learning function."""
        self._motor_learner = learner
        
    def plan_action(self, goal_state, current_state, constraints=None):
        """
        Plan an action sequence to reach a goal state.
        
        Args:
            goal_state: Target state to reach
            current_state: Current state
            constraints: Action constraints
            
        Returns:
            list: Planned action sequence
        """
        if self._action_planner:
            return self._action_planner(goal_state, current_state, constraints)
        else:
            # Default simple planning
            return [self._get_closest_action(goal_state)]
            
    def execute_action_sequence(self, action_sequence, current_state):
        """
        Execute a sequence of actions.
        
        Args:
            action_sequence: Sequence of actions to execute
            current_state: Current state
            
        Returns:
            dict: Execution results
        """
        execution_result = {
            'actions': [],
            'states': [],
            'success': True,
            'errors': []
        }
        
        current_state_copy = current_state.copy() if current_state else {}
        
        for action in action_sequence:
            try:
                # Execute action
                result = self._execute_action(action, current_state_copy)
                
                execution_result['actions'].append(action)
                execution_result['states'].append(current_state_copy.copy())
                
                # Update state
                if 'new_state' in result:
                    current_state_copy = result['new_state']
                    
                # Store action in history
                self._action_history.append({
                    'action': action,
                    'state': current_state_copy.copy(),
                    'result': result,
                    'timestamp': time.time() if 'time' in globals() else 0.0
                })
                
            except Exception as e:
                execution_result['success'] = False
                execution_result['errors'].append({
                    'action': action,
                    'error': str(e),
                    'timestamp': time.time() if 'time' in globals() else 0.0
                })
        
        self._last_action = action_sequence[-1] if action_sequence else None
        return execution_result
        
    def _execute_action(self, action, current_state):
        """Execute a single action."""
        # Default action execution logic
        if hasattr(action, 'getType'):
            action_type = action.getType()
            
            # Simulate action effects
            result = {
                'action_type': action_type,
                'success': True,
                'new_state': current_state.copy(),
                'effects': {}
            }
            
            # Apply action effects based on type
            if action_type == ActionType.MoveForward:
                result['new_state']['x'] = result['new_state'].get('x', 0) + 1.0
                result['new_state']['y'] = result['new_state'].get('y', 0)
                result['effects']['movement'] = (1.0, 0.0)
                
            elif action_type == ActionType.MoveBackward:
                result['new_state']['x'] = result['new_state'].get('x', 0) - 1.0
                result['effects']['movement'] = (-1.0, 0.0)
                
            elif action_type == ActionType.TurnLeft:
                result['new_state']['orientation'] = (result['new_state'].get('orientation', 0) + 90) % 360
                result['effects']['rotation'] = 90
                
            elif action_type == ActionType.TurnRight:
                result['new_state']['orientation'] = (result['new_state'].get('orientation', 0) - 90) % 360
                result['effects']['rotation'] = -90
                
            elif action_type == ActionType.Interact:
                result['effects']['interaction'] = True
                
            elif action_type == ActionType.Eat:
                result['effects']['consumption'] = 'food'
                result['new_state']['energy'] = result['new_state'].get('energy', 100) - 20
                
            elif action_type == ActionType.Drink:
                result['effects']['consumption'] = 'water'
                result['new_state']['hydration'] = result['new_state'].get('hydration', 100) - 15
                
            elif action_type == ActionType.Rest:
                result['effects']['recovery'] = True
                result['new_state']['energy'] = min(100, result['new_state'].get('energy', 0) + 30)
                
            elif action_type == ActionType.Wait:
                result['effects']['passivity'] = True
                
            return result
        
        return {'success': False, 'error': 'Unknown action type'}
        
    def _get_closest_action(self, goal_state):
        """Get closest action to goal state (simplified)."""
        # Default: return wait action
        from .core import ActionType
        return Action(ActionType.WAIT)
        
    def learn_motor_control(self, experience):
        """
        Learn from motor control experience.
        
        Args:
            experience: Experience data for learning
        """
        if self._motor_learner:
            return self._motor_learner(experience)
        else:
            print("[INFO] Motor learning not implemented - using default behavior")
            return False
            
    def get_action_statistics(self):
        """
        Get statistics about action execution.
        
        Returns:
            dict: Action statistics
        """
        if not self._action_history:
            return {}
        
        action_types = {}
        success_count = 0
        
        for record in self._action_history:
            action = record['action']
            if hasattr(action, 'getType'):
                action_type = action.getType()
                action_types[action_type] = action_types.get(action_type, 0) + 1
                
            if record.get('result', {}).get('success', True):
                success_count += 1
        
        return {
            'total_actions': len(self._action_history),
            'success_rate': success_count / len(self._action_history) if self._action_history else 0,
            'action_types': action_types,
            'last_action': self._last_action,
            'average_action_time': sum(r['timestamp'] for r in self._action_history) / len(self._action_history) if self._action_history else 0
        }
        
    def get_action_suggestions(self, context_state):
        """
        Get suggested actions based on context state.
        
        Args:
            context_state: Current context state
            
        Returns:
            list: Suggested actions
        """
        suggestions = []
        
        # Analyze context to suggest appropriate actions
        if context_state.get('energy', 100) < 30:
            from .core import ActionType
            suggestions.append(Action(ActionType.Rest))
            
        if context_state.get('position_risk', 0) > 0.7:
            suggestions.append(Action(ActionType.Wait))
            
        if context_state.get('goal_reached', False):
            suggestions.append(Action(ActionType.Rest))
            
        if not suggestions:
            from .core import ActionType
            suggestions.append(Action(ActionType.WAIT))
        
        return suggestions
        
    def set_motor_properties(self, properties):
        """
        Set motor controller properties.
        
        Args:
            properties: Motor properties dictionary
        """
        self._motor_properties.update(properties)
        print(f"[INFO] Motor properties updated: {properties}")
        
    def get_motor_properties(self):
        """
        Get motor controller properties.
        
        Returns:
            dict: Motor properties
        """
        return self._motor_properties.copy()
        
    def reset(self):
        """Reset motor controller."""
        self._action_history.clear()
        self._last_action = None
        print("[INFO] Motor controller reset")
        
    def export_action_history(self, filepath):
        """
        Export action history to file.
        
        Args:
            filepath: Path to export file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            with open(filepath, 'w') as f:
                json.dump(self._action_history, f, default=str)
                
            print(f"[INFO] Action history exported to {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to export action history: {e}")
            return False
            
    def import_action_history(self, filepath):
        """
        Import action history from file.
        
        Args:
            filepath: Path to import file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            with open(filepath, 'r') as f:
                self._action_history = json.load(f)
                
            print(f"[INFO] Action history imported from {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to import action history: {e}")
            return False
            
    def __str__(self):
        """String representation of motor controller."""
        return f"MotorController(actions={len(self._actions)}, history={len(self._action_history)})"
        
    def __repr__(self):
        """Detailed representation of motor controller."""
        return f"MotorController(actions={len(self._actions)}, planner={'set' if self._action_planner else 'None'}, " \
               f"learner={'set' if self._motor_learner else 'None'})"
# Export advanced motor classes and functions
from .motor import MotorController