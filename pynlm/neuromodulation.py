"""
Neuromodulation Module

This module provides neuromodulation system interfaces for the NLM Python bindings.
It includes NeuromodulationSystem and various neuromodulator classes that wrap the
underlying C++ neuromodulation components.
"""

"""
Neuromodulation Module

This module provides neuromodulation system interfaces for the NLM Python bindings.
It includes NeuromodulationSystem and various neuromodulator classes that wrap the
underlying C++ neuromodulation components.
"""

from typing import List, Optional, Dict, Any
import time

from .types import NeuronId, NeuronType, Timestamp, SimulationStep
from .brain import Brain

class NeuromodulationSystem:
    """
    Neuromodulation system that regulates brain function through chemical signals.
    
    Neuromodulation systems use chemical signals to modulate neural activity,
    influencing learning, attention, motivation, and other brain functions.
    
    Key neuromodulators:
    - Dopamine: Reward prediction and motivation
    - Curiosity: Exploration and novelty detection
    - Novelty: Response to new stimuli
    - Prediction Error: Learning from prediction errors
    
    Features:
    - Multi- neuromodulator support
    - Real-time modulation
    - Learning integration
    - Context-dependent modulation
    - Strength-based activation
    
    Examples:
        >>> from pynlm import NeuromodulationSystem, create_brain
        >>> 
        >>> brain = create_brain({"neuron_count": 1000})
        >>> neuromodulation = NeuromodulationSystem(brain)
        >>> 
        >>> # Apply reward signal
        >>> neuromodulation.apply_dopamine(reward=1.0, predicted=0.5)
        >>> 
        >>> # Apply curiosity signal
        >>> neuromodulation.apply_curiosity(novelty=0.8)
        >>> 
        >>> # Get current neuromodulation levels
        >>> dopamine = neuromodulation.get_dopamine_level()
        >>> curiosity = neuromodulation.get_curiosity_level()
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize neuromodulation system.
        
        Args:
            brain: Brain instance to connect to.
        """
        self._brain = brain
        self._initialized = False
        
        # Neuromodulator instances
        self._dopamine = Dopamine(brain)
        self._curiosity = Curiosity(brain)
        self._novelty = Novelty(brain)
        self._prediction_error = PredictionError(brain)
        
        if self._brain:
            self._initialize()
    
    def _initialize(self):
        """Initialize neuromodulation system."""
        self._initialized = True
    
    @property
    def dopamine(self) -> 'Dopamine':
        """Get dopamine neuromodulator."""
        return self._dopamine
    
    @property
    def curiosity(self) -> 'Curiosity':
        """Get curiosity neuromodulator."""
        return self._curiosity
    
    @property
    def novelty(self) -> 'Novelty':
        """Get novelty neuromodulator."""
        return self._novelty
    
    @property
    def prediction_error(self) -> 'PredictionError':
        """Get prediction error neuromodulator."""
        return self._prediction_error
    
    def apply_dopamine(self, reward: float, predicted_reward: float = 0.0,
                      context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply dopamine neuromodulation signal.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Expected reward (prediction).
            context: Optional contextual information.
        
        Returns:
            Dopamine level signal.
        """
        return self._dopamine.apply_signal(reward, predicted_reward, context)
    
    def apply_curiosity(self, novelty: float, prediction_error: float = 0.0,
                       context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply curiosity neuromodulation signal.
        
        Args:
            novelty: Novelty level of stimulus.
            prediction_error: Prediction error signal.
            context: Optional contextual information.
        
        Returns:
            Curiosity level signal.
        """
        return self._curiosity.apply_signal(novelty, prediction_error, context)
    
    def apply_novelty(self, novelty: float,
                     context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply novelty neuromodulation signal.
        
        Args:
            novelty: Novelty level of stimulus.
            context: Optional contextual information.
        
        Returns:
            Novelty level signal.
        """
        return self._novelty.apply_signal(novelty, context)
    
    def apply_prediction_error(self, error: float,
                              context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply prediction error neuromodulation signal.
        
        Args:
            error: Prediction error signal.
            context: Optional contextual information.
        
        Returns:
            Prediction error signal.
        """
        return self._prediction_error.apply_signal(error, context)
    
    def get_dopamine_level(self) -> float:
        """Get current dopamine level."""
        return self._dopamine.get_level()
    
    def get_curiosity_level(self) -> float:
        """Get current curiosity level."""
        return self._curiosity.get_level()
    
    def get_novelty_level(self) -> float:
        """Get current novelty level."""
        return self._novelty.get_level()
    
    def get_prediction_error_level(self) -> float:
        """Get current prediction error level."""
        return self._prediction_error.get_level()
    
    def get_neuromodulation_summary(self) -> Dict[str, float]:
        """
        Get summary of all neuromodulation levels.
        
        Returns:
            Dictionary with neuromodulator levels.
        """
        return {
            'dopamine': self.get_dopamine_level(),
            'curiosity': self.get_curiosity_level(),
            'novelty': self.get_novelty_level(),
            'prediction_error': self.get_prediction_error_level(),
        }
    
    def update(self, dt: float) -> None:
        """
        Update neuromodulation system.
        
        Args:
            dt: Time step for update.
        """
        self._dopamine.update(dt)
        self._curiosity.update(dt)
        self._novelty.update(dt)
        self._prediction_error.update(dt)
    
    def export_to_dataframe(self) -> 'pd.DataFrame':
        """
        Export neuromodulation data to Pandas DataFrame.
        
        Returns:
            Pandas DataFrame with neuromodulation data.
        
        Raises:
            ImportError: If pandas is not available.
        """
        try:
            import pandas as pd
        except ImportError:
            raise ImportError("Pandas is required for DataFrame export")
        
        summary = self.get_neuromodulation_summary()
        return pd.DataFrame([summary])

class Dopamine:
    """
    Dopamine neuromodulator for reward prediction and motivation.
    
    Dopamine signals encode prediction errors and reward prediction,
    driving learning and motivated behavior.
    
    Features:
    - Reward prediction
    - Prediction error signaling
    - Motivation modulation
    - Learning rate adjustment
    - Contextual modulation
    
    Examples:
        >>> dopamine = Dopamine(brain)
        >>> level = dopamine.apply_signal(reward=1.0, predicted=0.5)
        >>> print(f"Dopamine level: {level}")
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize dopamine neuromodulator.
        
        Args:
            brain: Brain instance to connect to.
        """
        self._brain = brain
        self._level = 0.0
        self._prediction_error = 0.0
        self._expected_reward = 0.0
        self._learning_rate = 0.1
        self._decay_rate = 0.9
        self._context_modulation = {}
    
    def apply_signal(self, reward: float, predicted_reward: float = 0.0,
                     context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply dopamine signal.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Expected reward (prediction).
            context: Optional contextual information.
        
        Returns:
            Dopamine level signal.
        """
        # Calculate prediction error
        self._prediction_error = reward - predicted_reward
        
        # Update expected reward (exponential moving average)
        self._expected_reward = (
            self._decay_rate * self._expected_reward + 
            (1.0 - self._decay_rate) * reward
        )
        
        # Apply context modulation if available
        if context:
            self._apply_context_modulation(context)
        
        # Set dopamine level based on prediction error
        self._level = self._prediction_error
        
        # Clamp to reasonable range
        self._level = max(-1.0, min(1.0, self._level))
        
        return self._level
    
    def _apply_context_modulation(self, context: Dict[str, Any]):
        """Apply contextual modulation to dopamine level."""
        for key, value in context.items():
            if key == 'stress':
                self._level *= (1.0 - value * 0.1)
            elif key == 'arousal':
                self._level *= (1.0 + value * 0.05)
            elif key == 'learning_phase':
                if value == 'exploration':
                    self._level *= 0.8
                elif value == 'exploitation':
                    self._level *= 1.2
    
    def update(self, dt: float) -> None:
        """
        Update dopamine level (decay).
        
        Args:
            dt: Time step for update.
        """
        self._level *= (self._decay_rate ** dt)
    
    def get_level(self) -> float:
        """Get current dopamine level."""
        return self._level
    
    def set_learning_rate(self, rate: float) -> None:
        """
        Set learning rate.
        
        Args:
            rate: Learning rate (0.0 to 1.0).
        """
        if not 0.0 <= rate <= 1.0:
            raise ValueError("Learning rate must be between 0.0 and 1.0")
        self._learning_rate = rate
    
    def get_prediction_error(self) -> float:
        """Get current prediction error."""
        return self._prediction_error

class Curiosity:
    """
    Curiosity neuromodulator for exploration and novelty detection.
    
    Curiosity drives exploration and information seeking behavior,
    balancing exploration with exploitation.
    
    Features:
    - Novelty detection
    - Exploration-exploitation tradeoff
    - Information gain
    - Behavioral flexibility
    - Intrinsic motivation
    
    Examples:
        >>> curiosity = Curiosity(brain)
        >>> level = curiosity.apply_signal(novelty=0.8, prediction_error=0.2)
        >>> print(f"Curiosity level: {level}")
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize curiosity neuromodulator.
        
        Args:
            brain: Brain instance to connect to.
        """
        self._brain = brain
        self._level = 0.0
        self._novelty_sensitivity = 0.5
        self._prediction_error_sensitivity = 0.3
        self._decay_rate = 0.95
        self._min_level = 0.0
        self._max_level = 1.0
    
    def apply_signal(self, novelty: float, prediction_error: float = 0.0,
                     context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply curiosity signal.
        
        Args:
            novelty: Novelty level of stimulus.
            prediction_error: Prediction error signal.
            context: Optional contextual information.
        
        Returns:
            Curiosity level signal.
        """
        # Calculate curiosity level based on novelty and prediction error
        novelty_contribution = novelty * self._novelty_sensitivity
        error_contribution = abs(prediction_error) * self._prediction_error_sensitivity
        
        self._level = novelty_contribution + error_contribution
        
        # Apply context modulation if available
        if context:
            self._apply_context_modulation(context)
        
        # Clamp to reasonable range
        self._level = max(self._min_level, min(self._max_level, self._level))
        
        return self._level
    
    def _apply_context_modulation(self, context: Dict[str, Any]):
        """Apply contextual modulation to curiosity level."""
        for key, value in context.items():
            if key == 'fatigue':
                self._level *= (1.0 - value * 0.2)
            elif key == 'curiosity_threshold':
                if value > 0.5:
                    self._level *= 0.7
            elif key == 'behavioral_flexibility':
                if value == 'high':
                    self._level *= 1.2
                elif value == 'low':
                    self._level *= 0.8
    
    def update(self, dt: float) -> None:
        """
        Update curiosity level (decay).
        
        Args:
            dt: Time step for update.
        """
        self._level *= (self._decay_rate ** dt)
    
    def get_level(self) -> float:
        """Get current curiosity level."""
        return self._level
    
    def set_novelty_sensitivity(self, sensitivity: float) -> None:
        """
        Set novelty sensitivity.
        
        Args:
            sensitivity: Novelty sensitivity (0.0 to 1.0).
        """
        if not 0.0 <= sensitivity <= 1.0:
            raise ValueError("Novelty sensitivity must be between 0.0 and 1.0")
        self._novelty_sensitivity = sensitivity

class Novelty:
    """
    Novelty neuromodulator for response to new stimuli.
    
    Novelty signals encode the difference between expected and actual stimuli,
    driving attention to new information.
    
    Features:
    - Difference encoding
    - Adaptation to novelty
    - Attention allocation
    - Memory updating
    - Learning acceleration
    
    Examples:
        >>> novelty = Novelty(brain)
        >>> level = novelty.apply_signal(novelty=0.8)
        >>> print(f"Novelty level: {level}")
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize novelty neuromodulator.
        
        Args:
            brain: Brain instance to connect to.
        """
        self._brain = brain
        self._level = 0.0
        self._decay_rate = 0.98
        self._adaptation_rate = 0.1
        self._last_stimulus = 0.0
        self._novelty_history = []
        self._max_history = 10
    
    def apply_signal(self, novelty: float,
                     context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply novelty signal.
        
        Args:
            novelty: Novelty level of stimulus.
            context: Optional contextual information.
        
        Returns:
            Novelty level signal.
        """
        # Calculate novelty based on difference from last stimulus
        if self._last_stimulus != 0.0:
            self._level = abs(novelty - self._last_stimulus)
        else:
            self._level = novelty
        
        # Update last stimulus
        self._last_stimulus = novelty
        
        # Add to history
        self._novelty_history.append(self._level)
        if len(self._novelty_history) > self._max_history:
            self._novelty_history.pop(0)
        
        # Apply context modulation if available
        if context:
            self._apply_context_modulation(context)
        
        # Decay
        self._level *= self._decay_rate
        
        return self._level
    
    def _apply_context_modulation(self, context: Dict[str, Any]):
        """Apply contextual modulation to novelty level."""
        for key, value in context.items():
            if key == 'familiarity':
                if value > 0.5:
                    self._level *= 0.5
            elif key == 'attention':
                self._level *= (1.0 + value * 0.3)
    
    def update(self, dt: float) -> None:
        """
        Update novelty level (adapt and decay).
        
        Args:
            dt: Time step for update.
        """
        # Adapt to recent novelty levels
        if self._novelty_history:
            recent_avg = sum(self._novelty_history) / len(self._novelty_history)
            self._level = (
                self._adaptation_rate * recent_avg + 
                (1.0 - self._adaptation_rate) * self._level
            )
        
        # Additional decay
        self._level *= (self._decay_rate ** dt)
    
    def get_level(self) -> float:
        """Get current novelty level."""
        return self._level

class PredictionError:
    """
    Prediction error neuromodulator for learning from mismatches.
    
    Prediction error signals encode the difference between expected and actual
    outcomes, driving learning and adjustment of expectations.
    
    Features:
    - Prediction error encoding
    - Learning rate modulation
    - Expectation updating
    - Adaptive learning
    - Error correction
    
    Examples:
        >>> pred_error = PredictionError(brain)
        >>> level = pred_error.apply_signal(error=0.5)
        >>> print(f"Prediction error level: {level}")
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize prediction error neuromodulator.
        
        Args:
            brain: Brain instance to connect to.
        """
        self._brain = brain
        self._level = 0.0
        self._learning_rate = 0.2
        self._decay_rate = 0.99
        self._error_history = []
        self._max_history = 5
    
    def apply_signal(self, error: float,
                     context: Optional[Dict[str, Any]] = None) -> float:
        """
        Apply prediction error signal.
        
        Args:
            error: Prediction error signal.
            context: Optional contextual information.
        
        Returns:
            Prediction error level signal.
        """
        # Store error in history
        self._error_history.append(abs(error))
        if len(self._error_history) > self._max_history:
            self._error_history.pop(0)
        
        # Apply context modulation if available
        if context:
            self._apply_context_modulation(context)
        
        # Set prediction error level
        self._level = abs(error)
        
        # Clamp to reasonable range
        self._level = min(1.0, self._level)
        
        return self._level
    
    def _apply_context_modulation(self, context: Dict[str, Any]):
        """Apply contextual modulation to prediction error level."""
        for key, value in context.items():
            if key == 'confidence':
                if value < 0.5:
                    self._level *= 1.5
            elif key == 'uncertainty':
                self._level *= (1.0 + value * 0.5)
    
    def update(self, dt: float) -> None:
        """
        Update prediction error level (decay and adapt).
        
        Args:
            dt: Time step for update.
        """
        # Decay
        self._level *= (self._decay_rate ** dt)
        
        # Adapt based on recent errors
        if self._error_history:
            recent_avg = sum(self._error_history) / len(self._error_history)
            self._level = (
                self._learning_rate * recent_avg + 
                (1.0 - self._learning_rate) * self._level
            )
    
    def get_level(self) -> float:
        """Get current prediction error level."""
        return self._level

# Export commonly used symbols

__all__ = [
    'NeuromodulationSystem', 'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
]
