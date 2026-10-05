"""
ActionResult class for NLM neural simulation framework.

This module provides a Python interface to the C++ ActionResult class with enhanced
Python features including automatic numpy conversion, error formatting, and
time measurement utilities.
"""

import time
import numpy as np
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
import traceback

# Python utilities
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
)

@dataclass
class ActionResultState:
    """State information for action result"""
    reward: float = 0.0
    success: bool = False
    message: str = ""
    timestamp: float = 0.0

class ActionResult:
    """
    ActionResult class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ ActionResult class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """
        Initialize action result.
        
        Args:
            reward: Reward value (default: 0.0)
            success: Success status (default: False)
            message: Result message (default: "")
        """
        self._logger = PythonSessionLogger()
        self._state = ActionResultState()
        
        # Set state
        self._state.reward = reward
        self._state.success = success
        self._state.message = message
        self._state.timestamp = time.time()
        
        # Log initialization
        self._logger.logEntry(f"ActionResult initialized: reward={reward}, success={success}, message='{message}'")
    
    # Reward property
    
    @property
    def reward(self) -> float:
        """Get reward value."""
        return self._state.reward
    
    @reward.setter
    def reward(self, value: float) -> None:
        """
        Set reward value.
        
        Args:
            value: Reward value
        """
        self._state.reward = value
    
    # Success property
    
    @property
    def success(self) -> bool:
        """Get success status."""
        return self._state.success
    
    @success.setter
    def success(self, value: bool) -> None:
        """
        Set success status.
        
        Args:
            value: Success status
        """
        self._state.success = value
    
    # Message property
    
    @property
    def message(self) -> str:
        """Get result message."""
        return self._state.message
    
    @message.setter
    def message(self, value: str) -> None:
        """
        Set result message.
        
        Args:
            value: Result message
        """
        self._state.message = value
    
    # Timestamp property
    
    @property
    def timestamp(self) -> float:
        """Get timestamp."""
        return self._state.timestamp
    
    @timestamp.setter
    def timestamp(self, value: float) -> None:
        """
        Set timestamp.
        
        Args:
            value: Timestamp
        """
        self._state.timestamp = value
    
    # Action result operations
    
    def setSuccess(self, success: bool, message: str = "") -> None:
        """
        Set action result success and message.
        
        Args:
            success: Success status
            message: Result message
        """
        self._state.success = success
        self._state.message = message
        self._state.timestamp = time.time()
        
        self._logger.logEntry(f"ActionResult set: success={success}, message='{message}'")
    
    def setReward(self, reward: float) -> None:
        """
        Set action result reward.
        
        Args:
            reward: Reward value
        """
        self._state.reward = reward
        self._state.timestamp = time.time()
        
        self._logger.logEntry(f"ActionResult reward set: {reward}")
    
    def setMessage(self, message: str) -> None:
        """
        Set action result message.
        
        Args:
            message: Result message
        """
        self._state.message = message
        self._state.timestamp = time.time()
        
        self._logger.logEntry(f"ActionResult message set: '{message}'")
    
    def getValue(self) -> float:
        """
        Get action result value (backward compatibility).
        
        Returns:
            Reward value
        """
        return self._state.reward
    
    def getSuccess(self) -> bool:
        """
        Get action result success (backward compatibility).
        
        Returns:
            Success status
        """
        return self._state.success
    
    def getMessage(self) -> str:
        """
        Get action result message (backward compatibility).
        
        Returns:
            Result message
        """
        return self._state.message
    
    def getTimestamp(self) -> float:
        """
        Get action result timestamp.
        
        Returns:
            Timestamp
        """
        return self._state.timestamp
    
    def reset(self) -> None:
        """Reset action result."""
        self._state.reward = 0.0
        self._state.success = False
        self._state.message = ""
        self._state.timestamp = time.time()
        
        self._logger.logEntry("ActionResult reset")
    
    def __repr__(self) -> str:
        """String representation of action result."""
        return f"<ActionResult: reward={self._state.reward}, success={self._state.success}, message='{self._state.message}'>"
    
    def __str__(self) -> str:
        """String representation of action result."""
        return f"NLM ActionResult: reward={self._state.reward}, success={self._state.success}, message='{self._state.message}'"

__all__ = ['ActionResult']
