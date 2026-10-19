"""
NLM Motor Module

This module contains motor system components including action generation and
motor command processing. These systems translate neural activity into actions.
"""

from .motor_system import MotorSystem
from .action import Action

__all__ = ['MotorSystem', 'Action']
