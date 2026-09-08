"""
Sensory processing module for NLM.

This module provides classes and functions for processing sensory input
from the environment, including feature extraction, normalization, and
encoding for neural processing.
"""

from typing import Dict, Any, Optional, List, Tuple
from enum import Enum
import numpy as np
from contextlib import contextmanager

from .exceptions import (
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
    ResourceError,
)

from .world import WorldObject, WorldObjectType
class SensoryPercept:
    """
    Represents sensory input from the environment.
    
    This class encapsulates sensory data from the world, including
    features, internal state, and contextual information.
    """

    def __init__(
        self,
        x: float,
        y: float,
        world_width: int,
        world_height: int,
        objects: Optional[List[WorldObject]] = None,
    ):
        """
        Initialize a new sensory percept.
        
        Args:
            x: Agent X coordinate
            y: Agent Y coordinate
            world_width: World width
            world_height: World height
            objects: List of world objects
        """
        self._x = x
        self._y = y
        self._world_width = world_width
        self._world_height = world_height
        self._objects = objects or []
        self._features = self._extract_features()
        self._internal_state = self._extract_internal_state()

    def __repr__(self) -> str:
        """String representation."""
        return f"SensoryPercept(x={self._x}, y={self._y}, features={len(self._features)})"

    def _extract_features(self) -> Dict[str, Any]:
        """Extract features from world state."""
        features = {}

        # Distance to nearest object
        min_distance = float('inf')
        for obj in self._objects:
            distance = ((obj.x - self._x) ** 2 + (obj.y - self._y) ** 2) ** 0.5
            if distance < min_distance:
                min_distance = distance

        features["nearest_object_distance"] = min_distance if min_distance != float('inf') else float('inf')

        # Object count by type
        object_counts = {}
        for obj_type in WorldObjectType:
            object_counts[obj_type.value] = sum(1 for obj in self._objects if obj.type == obj_type)

        features["object_counts"] = object_counts

        # Position relative to world bounds
        features["x_relative"] = self._x / self._world_width if self._world_width > 0 else 0.0
        features["y_relative"] = self._y / self._world_height if self._world_height > 0 else 0.0

        return features

    def _extract_internal_state(self) -> Dict[str, Any]:
        """Extract internal state."""
        return {
            "energy_level": 1.0,  # Simulated energy level
            "time_of_day": "day",  # Simulated time
        }

    # Properties for Pythonic interface

    @property
    def x(self) -> float:
        """Get X coordinate."""
        return self._x

    @property
    def y(self) -> float:
        """Get Y coordinate."""
        return self._y

    @property
    def features(self) -> Dict[str, Any]:
        """Get extracted features."""
        return self._features.copy()

    @property
    def internal(self) -> Dict[str, Any]:
        """Get internal state."""
        return self._internal_state.copy()

    @property
    def objects(self) -> List[WorldObject]:
        """Get world objects."""
        return self._objects.copy()

    def get_features(self) -> Dict[str, Any]:
        """Get extracted features."""
        return self.features

    def get_internal(self) -> Dict[str, Any]:
        """Get internal state."""
        return self.internal


class SensoryProcessor:
    """
    Processes and encodes sensory input for neural processing.
    
    This class provides functions for preprocessing sensory data,
    extracting features, and preparing input for neural networks.
    """

    def __init__(self):
        """Initialize a new SensoryProcessor instance."""
        self._initialized = False
        self._config = {}

    def __enter__(self):
        """Context manager entry point."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.reset()

    def initialize(self, config: Optional[Dict[str, Any]] = None) -> None:
        """
        Initialize the sensory processor.
        
        Args:
            config: Optional configuration dictionary
        """
        self._config = config or {}
        self._initialized = True

    def process(self, percept: SensoryPercept) -> Dict[str, Any]:
        """
        Process sensory percept.
        
        Args:
            percept: Sensory percept to process
            
        Returns:
            Dictionary with processed sensory information
        """
        if not self._initialized:
            raise InitializationError("SensoryProcessor not initialized. Call initialize() first.")

        # Extract and normalize features
        features = percept.get_features()
        processed_features = self._normalize_features(features)

        return {
            "original_percept": percept,
            "processed_features": processed_features,
            "encoding": self._encode_features(processed_features),
        }

    def reset(self) -> None:
        """Reset sensory processor."""
        self._initialized = False
        self._config = {}

    # Processing methods

    def _normalize_features(self, features: Dict[str, Any]) -> Dict[str, Any]:
        """Normalize features."""
        normalized = {}

        for key, value in features.items():
            if isinstance(value, (int, float)):
                normalized[key] = self._normalize_value(value)
            elif isinstance(value, dict):
                normalized[key] = {k: self._normalize_value(v) for k, v in value.items()}
            else:
                normalized[key] = value

        return normalized

    def _normalize_value(self, value: Union[int, float]) -> float:
        """Normalize a single value."""
        # Apply simple normalization
        if isinstance(value, (int, float)):
            return float(value) / 1000.0  # Normalize to [0, 1] range

        return value

    def _encode_features(self, features: Dict[str, Any]) -> np.ndarray:
        """Encode features to vector."""
        # Simple feature encoding
        feature_vector = []

        for key, value in features.items():
            if isinstance(value, (int, float)):
                feature_vector.append(value)
            elif isinstance(value, dict):
                feature_vector.extend(value.values())

        return np.array(feature_vector)

    # Properties for Pythonic interface

    @property
    def is_initialized(self) -> bool:
        """Check if sensory processor is initialized."""
        return self._initialized

    @property
    def config(self) -> Dict[str, Any]:
        """Get sensory processor configuration."""
        return self._config.copy()