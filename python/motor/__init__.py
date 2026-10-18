"""
Motor module for NLM

This module provides motor action classes for NLM.
"""

from .core import ActionType, Action
from typing import List

class ActionResult:
    def __init__(self):
        self._reward = 0.0
        self._success = False
        self._message = ""

    def getReward(self) -> float:
        return self._reward

    def setReward(self, reward: float):
        self._reward = reward

    def getSuccess(self) -> bool:
        return self._success

    def setSuccess(self, success: bool):
        self._success = success

    def getMessage(self) -> str:
        return self._message

    def setMessage(self, message: str):
        self._message = message

    def __repr__(self):
        return f"ActionResult(reward={self._reward}, success={self._success})"

# Convenience function

def createActionResult(reward: float = 0.0, success: bool = False, message: str = "") -> ActionResult:
    return ActionResult()

__all__ = [
    'ActionResult',
    'createActionResult',
]