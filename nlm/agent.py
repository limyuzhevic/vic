"""
Agent operations for NLM.

This module provides Pythonic access to agent brain operations,
including advanced actions, action sequences, and agent functionality.
"""

import time
from typing import Any, Dict, List, Optional, Union, Iterator, ContextManager, Callable
from contextlib import contextmanager

from ..bindings import (
    Action, ActionType, ActionResult, ActionResultStatus,
    ActionParameter, ActionTemplate, ActionSequence,
    AdvancedActions, ActionValidator, ActionRecovery
)

from .exceptions import NLMError, SimulationError

__all__ = [
    # Action classes
    "Action", "ActionType", "ActionResult", "ActionResultStatus",
    "ActionParameter", "ActionTemplate", "ActionSequence",
    # Advanced actions
    "AdvancedActions", "ActionValidator", "ActionRecovery",
    # Factory functions
    "create_action", "create_action_from_template",
    # Context managers
    "action_context", "sequence_context",
    # Exceptions
    "NLMError", "SimulationError",
]

# Pythonic context managers
@contextmanager
def action_context(action_type: ActionType, parameters: Optional[Dict] = None) -> Iterator[Action]:
    """
    Context manager for action execution.
    
    Args:
        action_type: Type of action to create.
        parameters: Optional parameters for the action.
    
    Yields:
        An Action instance that can be executed.
    """
    action = create_action(action_type, parameters)
    try:
        yield action
    finally:
        # Cleanup can be done here if needed
        pass

@contextmanager
def sequence_context(name: str, actions: Optional[List[Action]] = None) -> Iterator[ActionSequence]:
    """
    Context manager for action sequence execution.
    
    Args:
        name: Name for the action sequence.
        actions: Optional list of actions to add to the sequence.
    
    Yields:
        An ActionSequence instance.
    """
    sequence = create_action_sequence(name)
    if actions:
        for action in actions:
            sequence.add_action(action)
    
    try:
        yield sequence
    finally:
        # Cleanup can be done here if needed
        pass

# Factory functions
import itertools
from itertools import count
def create_action(action_type: ActionType, parameters: Optional[Dict] = None) -> Action:
    """
    Create an action with the specified type and parameters.
    
    Args:
        action_type: Type of action to create.
        parameters: Optional dictionary of action parameters.
    
    Returns:
        An Action instance.
    
    Raises:
        NLMError: If action creation fails.
    """
    try:
        if parameters:
            action = Action(action_type, parameters)
        else:
            action = Action(action_type)
        return action
    except Exception as e:
        raise SimulationError(f"Failed to create action: {str(e)}")

def create_action_sequence(name: str) -> ActionSequence:
    """
    Create an action sequence with the specified name.
    
    Args:
        name: Name for the action sequence.
    
    Returns:
        An ActionSequence instance.
    
    Raises:
        NLMError: If action sequence creation fails.
    """
    try:
        return ActionSequence(name)
    except Exception as e:
        raise SimulationError(f"Failed to create action sequence: {str(e)}")

def create_action_template(name: str, description: str = "") -> ActionTemplate:
    """
    Create an action template with the specified name and description.
    
    Args:
        name: Name for the action template.
        description: Optional description for the template.
    
    Returns:
        An ActionTemplate instance.
    
    Raises:
        NLMError: If action template creation fails.
    """
    try:
        return ActionTemplate(name, description)
    except Exception as e:
        raise SimulationError(f"Failed to create action template: {str(e)}")

def create_advanced_actions() -> AdvancedActions:
    """
    Create an advanced actions manager.
    
    Returns:
        An AdvancedActions instance.
    
    Raises:
        NLMError: If advanced actions creation fails.
    """
    try:
        return AdvancedActions()
    except Exception as e:
        raise SimulationError(f"Failed to create advanced actions: {str(e)}")

# Enhanced action classes with Pythonic API
class EnhancedAction:
    """
    Enhanced Python API for the Action class with modern Python patterns.
    
    Provides property-based access, Pythonic string representations,
    and convenient methods for action manipulation.
    """
    
    def __init__(self, action: Action):
        self._action = action
    
    @property
    def action_type(self) -> ActionType:
        """Get the action type."""
        return self._action.getType()
    
    @property
    def parameters(self) -> List[float]:
        """Get the action parameters."""
        return self._action.getParameters()
    
    @property
    def name(self) -> str:
        """Get the action name."""
        return self._action.getName()
    
    def set_type(self, action_type: ActionType) -> None:
        """Set the action type."""
        self._action.setType(action_type)
    
    def set_parameters(self, parameters: List[float]) -> None:
        """Set the action parameters."""
        self._action.setParameters(parameters)
    
    def execute(self) -> ActionResult:
        """
        Execute the action.
        
        Returns:
            The action result.
        """
        # Note: This assumes the Action class has an execute method
        # or we need to integrate with the AdvancedActions system
        return ActionResult()
    
    def clone(self) -> 'EnhancedAction':
        """Create a copy of this action."""
        cloned_action = Action(self.action_type, self.parameters)
        return EnhancedAction(cloned_action)
    
    def __repr__(self) -> str:
        """Get a string representation of the action."""
        return f"<EnhancedAction type={self.action_type} parameters={self.parameters}>"
    
    def __str__(self) -> str:
        """Get a human-readable string representation."""
        return f"Action(type={self.action_type}, params={self.parameters})"

# Enhanced action sequence class
class EnhancedActionSequence:
    """
    Enhanced Python API for ActionSequence with modern Python patterns.
    
    Provides iteration support, Pythonic method chaining,
    and convenient methods for sequence management.
    """
    
    def __init__(self, sequence: ActionSequence):
        self._sequence = sequence
    
    @property
    def name(self) -> str:
        """Get the sequence name."""
        return self._sequence.getName()
    
    @property
    def action_count(self) -> int:
        """Get the number of actions in the sequence."""
        return len(self._sequence.getActions())
    
    @property
    def is_executing(self) -> bool:
        """Check if the sequence is currently executing."""
        return self._sequence.isExecuting()
    
    def add_action(self, action: Action, optional: bool = False) -> None:
        """
        Add an action to the sequence.
        
        Args:
            action: The action to add.
            optional: Whether the action is optional.
        """
        self._sequence.addAction(action, optional)
    
    def get_actions(self) -> Iterator[Action]:
        """
        Get all actions in the sequence.
        
        Yields:
            Action objects in the sequence.
        """
        for action in self._sequence.getActions():
            yield action
    
    def execute(self, tags: Optional[List[str]] = None) -> List[ActionResult]:
        """
        Execute the action sequence.
        
        Args:
            tags: Optional tags for filtering actions.
        
        Returns:
            List of action results.
        """
        if tags is None:
            return self._sequence.execute()
        else:
            return self._sequence.execute(set(tags))
    
    def clear(self) -> None:
        """Clear all actions from the sequence."""
        # Note: This assumes ActionSequence has a clear method
        pass
    
    def get_results(self) -> List[ActionResult]:
        """
        Get the results of the last execution.
        
        Returns:
            List of action results.
        """
        return self._sequence.getResults()
    
    def __repr__(self) -> str:
        """Get a string representation of the sequence."""
        return f"<EnhancedActionSequence name={self.name} actions={self.action_count}>"
    
    def __str__(self) -> str:
        """Get a human-readable string representation."""
        return f"ActionSequence(name={self.name}, actions={self.action_count})"

# Enhanced advanced actions class
class EnhancedAdvancedActions:
    """
    Enhanced Python API for AdvancedActions with modern Python patterns.
    
    Provides Pythonic access to advanced action functionality,
    including action templates, sequences, and execution management.
    """
    
    def __init__(self, advanced_actions: AdvancedActions):
        self._advanced_actions = advanced_actions
    
    def register_template(self, template: ActionTemplate) -> None:
        """
        Register an action template.
        
        Args:
            template: The action template to register.
        """
        self._advanced_actions.registerTemplate(template)
    
    @property
    def templates(self) -> Iterator[ActionTemplate]:
        """
        Get all registered templates.
        
        Yields:
            ActionTemplate objects.
        """
        for template in self._advanced_actions.getTemplates():
            yield template
    
    def create_from_template(self, template_name: str, 
                             parameters: Optional[Dict] = None) -> Action:
        """
        Create an action from a template.
        
        Args:
            template_name: Name of the template to use.
            parameters: Optional parameters for the action.
        
        Returns:
            An Action instance created from the template.
        """
        if parameters is None:
            parameters = {}
        
        return self._advanced_actions.createFromTemplate(template_name, parameters)
    
    def create_sequence(self, name: str) -> EnhancedActionSequence:
        """
        Create an action sequence.
        
        Args:
            name: Name for the sequence.
        
        Returns:
            An EnhancedActionSequence instance.
        """
        sequence = self._advanced_actions.createSequence(name)
        return EnhancedActionSequence(sequence)
    
    def execute_action(self, action: Action, 
                      validator: Optional[ActionValidator] = None) -> ActionResult:
        """
        Execute a single action.
        
        Args:
            action: The action to execute.
            validator: Optional validator for the action.
        
        Returns:
            The action result.
        """
        if validator is None:
            validator = ActionValidator()
        
        return self._advanced_actions.executeAction(action, validator)
    
    def execute_action_with_recovery(self, action: Action,
                                     recovery: Optional[ActionRecovery] = None) -> ActionResult:
        """
        Execute a single action with recovery support.
        
        Args:
            action: The action to execute.
            recovery: Optional recovery strategy.
        
        Returns:
            The action result.
        """
        if recovery is None:
            recovery = ActionRecovery()
        
        return self._advanced_actions.executeActionWithRecovery(action, recovery)
    
    def execute_sequence(self, sequence_name: str) -> List[ActionResult]:
        """
        Execute an action sequence by name.
        
        Args:
            sequence_name: Name of the sequence to execute.
        
        Returns:
            List of action results.
        """
        return self._advanced_actions.executeSequence(sequence_name)
    
    def register_callback(self, callback_id: str, 
                          callback: Callable[[ActionResult], None]) -> None:
        """
        Register a callback for action execution.
        
        Args:
            callback_id: Unique identifier for the callback.
            callback: Function to call when an action is executed.
        """
        self._advanced_actions.registerCallback(callback_id, callback)
    
    def unregister_callback(self, callback_id: str) -> None:
        """
        Unregister a callback.
        
        Args:
            callback_id: Identifier of the callback to unregister.
        """
        self._advanced_actions.unregisterCallback(callback_id)
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get execution statistics.
        
        Returns:
            Dictionary of statistics.
        """
        return self._advanced_actions.getStats()
    
    def __repr__(self) -> str:
        """Get a string representation of the advanced actions."""
        return f"<EnhancedAdvancedActions templates={sum(1 for _ in self.templates)}>"
    
    def __str__(self) -> str:
        """Get a human-readable string representation."""
        return "EnhancedAdvancedActions"

# Type hints
from typing import Callable

__all__.extend([
    "EnhancedAction",
    "EnhancedActionSequence",
    "EnhancedAdvancedActions",
    "create_action",
    "create_action_sequence",
    "create_action_template",
    "create_advanced_actions",
    "action_context",
    "sequence_context",
])