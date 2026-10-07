# Neuromodulation system implementation for NLM Python API

"""
Neuromodulation system for NLM Python API

Provides Pythonic interfaces to neuromodulation system:
Dopamine, Curiosity, Novelty, PredictionError classes.
"""

from typing import Any, Optional, List
from enum import Enum

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class NeuromodulationError(NLMError):
    """Raised when neuromodulation operations fail"""
    pass

class Neuromodulator:
    """
    Base neuromodulator class
    
    Abstract base class for all neuromodulators.
    Provides common functionality for neuromodulation signals.
    
    Neuromodulators are chemical signals that affect neural activity,
    plasticity, and behavior in the NLM brain system.
    """
    
    def __init__(self, name: str):
        """
        Initialize neuromodulator.
        
        Args:
            name: Name of neuromodulator
        """
        self.name = name
        self._level = 0.0
        self._initialized = True
    
    def get_level(self) -> float:
        """
        Get current neuromodulator level.
        
        Returns:
            Current level (0.0 to 1.0 typical range)
        """
        return self._level
    
    def set_level(self, level: float) -> None:
        """
        Set neuromodulator level.
        
        Args:
            level: Level value (typically 0.0 to 1.0)
        """
        self._level = max(0.0, min(1.0, float(level)))
    
    def is_active(self) -> bool:
        """
        Check if neuromodulator is active.
        
        Returns:
            True if neuromodulator is active (level > 0)
        """
        return self._level > 0.0
    
    def get_name(self) -> str:
        """
        Get neuromodulator name.
        
        Returns:
            Neuromodulator name
        """
        return self.name

class Dopamine(Neuromodulator):
    """
    Dopamine neuromodulator
    
    Represents reward and reinforcement signaling in the NLM brain.
    Dopamine levels increase with reward prediction and actual rewards,
    signaling which behaviors are valuable for learning.
    
    High dopamine indicates:
    - Reward prediction accuracy
    - Novelty detection
    - Motivation to repeat rewarding behaviors
    - Increased plasticity for rewarded actions
    
    Low dopamine indicates:
    - Lack of reward
    - Punishment
    - Decreased motivation
    - Reduced plasticity
    
    Examples:
        >>> dopamine = Dopamine()
        >>> dopamine.setLevel(0.8)
        >>> level = dopamine.getLevel()
        >>> print(f"Dopamine level: {level}")
        >>> if dopamine.isActive():
        ...     print("Agent is motivated")
    """
    
    def __init__(self):
        """Initialize dopamine neuromodulator."""
        super().__init__("Dopamine")
    
    def predict_reward(self, expected_reward: float, actual_reward: float) -> float:
        """
        Compute reward prediction error (dopamine response).
        
        Args:
            expected_reward: Predicted reward
            actual_reward: Actual received reward
            
        Returns:
            Reward prediction error (positive = better than expected)
            
        Examples:
            >>> dopamine = Dopamine()
            >>> # Got reward when expecting 0.5, actually got 1.0
            >>> dpe = dopamine.predictReward(0.5, 1.0)
            >>> print(f"DPE: {dpe}")  # Should be +0.5
        """
        return actual_reward - expected_reward
    
    def get_learning_modulation(self) -> float:
        """
        Get learning modulation factor from dopamine.
        
        Returns:
            Learning modulation factor (0.0 to 2.0 typical range)
            
        Examples:
            >>> dopamine = Dopamine()
            >>> dopamine.setLevel(0.8)
            >>> learning_factor = dopamine.getLearningModulation()
            >>> print(f"Learning factor: {learning_factor:.2f}")
        """
        # Base learning rate on dopamine level
        # Higher dopamine = more learning
        if self._level < 0.3:
            return 0.5 + (self._level * 0.5)  # Low: 0.5-1.0
        elif self._level < 0.7:
            return 1.0 + ((self._level - 0.3) * 1.0)  # Medium: 1.0-1.7
        else:
            return 1.7 + ((self._level - 0.7) * 1.3)  # High: 1.7-2.0

class Curiosity(Neuromodulator):
    """
    Curiosity neuromodulator
    
    Represents exploration motivation in the NLM brain.
    Curiosity drives the agent to explore novel situations and
    learn about unknown aspects of the environment.
    
    High curiosity indicates:
    - Novelty detection
    - Exploration motivation
    - Increased attention to new stimuli
    - Reduced exploitation of known strategies
    
    Low curiosity indicates:
    - Preference for familiar situations
    - Exploitation of known strategies
    - Reduced exploration
    
    Examples:
        >>> curiosity = Curiosity()
        >>> curiosity.setLevel(0.9)
        >>> level = curiosity.getLevel()
        >>> print(f"Curiosity level: {level}")
        >>> if curiosity.isActive():
        ...     print("Agent will explore")
    """
    
    def __init__(self):
        """Initialize curiosity neuromodulator."""
        super().__init__("Curiosity")
    
    def get_exploration_bonus(self, state_history: Optional[List[float]] = None) -> float:
        """
        Get exploration bonus based on state history.
        
        Args:
            state_history: History of visited states (optional)
            
        Returns:
            Exploration bonus (0.0 to 1.0)
            
        Examples:
            >>> curiosity = Curiosity()
            >>> curiosity.setLevel(0.8)
            >>> bonus = curiosity.getExplorationBonus()
            >>> print(f"Exploration bonus: {bonus:.2f}")
        """
        base_bonus = self._level * 0.5
        
        # Reduce bonus if agent has explored a lot
        if state_history is not None and len(state_history) > 10:
            unique_states = len(set(state_history))
            exploration_ratio = unique_states / len(state_history)
            base_bonus *= max(0.1, 1.0 - exploration_ratio)
        
        return min(1.0, base_bonus)

class Novelty(Neuromodulator):
    """
    Novelty neuromodulator
    
    Represents novelty detection in the NLM brain.
    Novelty signals indicate when the agent encounters
    unexpected or surprising sensory input.
    
    High novelty indicates:
    - Sudden changes in sensory input
    - Unexpected objects or events
    - Information-rich situations
    - Learning opportunities
    
    Low novelty indicates:
    - Predictable environment
    - Familiar situations
    - Routine processing
    - Stable patterns
    
    Examples:
        >>> novelty = Novelty()
        >>> novelty.setLevel(0.6)
        >>> level = novelty.getLevel()
        >>> print(f"Novelty level: {level}")
        >>> if novelty.isActive():
        ...     print("Agent encountered something novel")
    """
    
    def __init__(self):
        """Initialize novelty neuromodulator."""
        super().__init__("Novelty")
    
    def detect_novelty(self, current_input: List[float], history: Optional[List[List[float]]] = None) -> float:
        """
        Detect novelty in sensory input.
        
        Args:
            current_input: Current sensory input
            history: History of previous inputs (optional)
            
        Returns:
            Novelty score (0.0 to 1.0)
            
        Examples:
            >>> novelty = Novelty()
            >>> current = [0.1, 0.9, 0.2]
            >>> history = [[0.1, 0.8, 0.1], [0.2, 0.7, 0.3]]
            >>> score = novelty.detectNovelty(current, history)
            >>> print(f"Novelty score: {score:.2f}")
        """
        if not current_input:
            return 0.0
        
        # Simple novelty detection based on deviation from history
        if history is None or len(history) < 2:
            # No history = maximum novelty
            return min(1.0, self._level)
        
        # Calculate distance from previous states
        from statistics import mean
        avg_input = [mean(channel) for channel in zip(*history)]
        
        # Euclidean distance from average
        distance_squared = sum((curr - avg) ** 2 for curr, avg in zip(current_input, avg_input))
        distance = distance_squared ** 0.5
        
        # Normalize distance (simple approximation)
        max_distance = len(current_input) ** 0.5
        novelty_score = min(1.0, distance / max_distance if max_distance > 0 else 0.0)
        
        return novelty_score * min(1.0, self._level)

class PredictionError(Neuromodulator):
    """
    Prediction error neuromodulator
    
    Represents prediction error signaling in the NLM brain.
    Prediction error indicates when the agent's expectations
    don't match reality, driving learning and adaptation.
    
    High prediction error indicates:
    - Large discrepancies between predicted and actual outcomes
    - Significant learning opportunities
    - Need for behavioral adjustment
    - Cognitive restructuring
    
    Low prediction error indicates:
    - Accurate predictions
    - Stable environment
    - Well-adapted behavior
    - Little need for change
    
    Examples:
        >>> prediction_error = PredictionError()
        >>> prediction_error.setLevel(0.7)
        >>> level = prediction_error.getLevel()
        >>> print(f"Prediction error level: {level}")
        >>> if prediction_error.isActive():
        ...     print("Agent needs to adjust behavior")
    """
    
    def __init__(self):
        """Initialize prediction error neuromodulator."""
        super().__init__("PredictionError")
    
    def compute_error(self, predicted: float, actual: float) -> float:
        """
        Compute prediction error.
        
        Args:
            predicted: Predicted value
            actual: Actual value
            
        Returns:
            Prediction error (positive = actual > predicted)
            
        Examples:
            >>> prediction_error = PredictionError()
            >>> # Predicted 0.5 but got 0.8
            >>> error = prediction_error.computeError(0.5, 0.8)
            >>> print(f"Prediction error: {error}")  # Should be 0.3
        """
        return actual - predicted
    
    def get_adaptation_strength(self, error_magnitude: float) -> float:
        """
        Get adaptation strength based on error magnitude.
        
        Args:
            error_magnitude: Magnitude of prediction error
            
        Returns:
            Adaptation strength (0.0 to 1.0)
            
        Examples:
            >>> prediction_error = PredictionError()
            >>> adaptation = prediction_error.getAdaptationStrength(0.5)
            >>> print(f"Adaptation strength: {adaptation:.2f}")
        """
        # Higher error magnitude = stronger adaptation
        adaptation = min(1.0, error_magnitude * 2.0)
        return adaptation * min(1.0, self._level)

# Factory functions for neuromodulators
def create_dopamine() -> Dopamine:
    """
    Create a dopamine neuromodulator.
    
    Returns:
        New Dopamine instance
    """
    return Dopamine()

def create_curiosity() -> Curiosity:
    """
    Create a curiosity neuromodulator.
    
    Returns:
        New Curiosity instance
    """
    return Curiosity()

def create_novelty() -> Novelty:
    """
    Create a novelty neuromodulator.
    
    Returns:
        New Novelty instance
    """
    return Novelty()

def create_prediction_error() -> PredictionError:
    """
    Create a prediction error neuromodulator.
    
    Returns:
        New PredictionError instance
    """
    return PredictionError()