"""
NLM Motor Module

This module contains motor-related classes and functionality:
- Action: Motor action representation
- MotorCommand: Motor command representation
"""

# Import motor classes from their respective modules
from .action import Action
from .motor_command import MotorCommand

__all__ = ['Action', 'MotorCommand']