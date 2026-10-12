#!/usr/bin/env python3

"""
NLM Serialization Module - State Management and Checkpointing

This module provides robust serialization capabilities:
- Checkpoint system for saving brain states
- State management with versioning and rollback
- Serialization utilities for experiment data
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export serialization classes for easy access
from nlm.serialization.checkpoint import CheckpointSystem
from nlm.serialization.state import StateManager

__all__ = [
    "CheckpointSystem", "StateManager",
    "__version__", "__author__"
]