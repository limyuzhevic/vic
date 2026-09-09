"""
NLM Python Neuromodulation Module

This module contains neuromodulation components for the NLM Python bindings.
"""

import numpy as np
from typing import Optional, List, Dict, Any
from .exceptions import NLMError, NeuromodulationError
from .core import Timestamp, TimestepDuration, NeuronId, NeuronType

# Import base classes
try:
    from ..brain import Brain
    from ..environment import SensoryInput
    _bindings_available = True
except ImportError:
    _bindings_available = False

class Neuromodulator:
    """Base class for neuromodulators."""
    
    def __init__(self):
        self.level = 0.0
        self.enabled = False
        self.brain = None
        self._neuromod_ptr = None
        
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                self._neuromod_ptr = None
    
    def initialize(self, brain):
        """Initialize neuromodulator with brain reference."""
        self.brain = brain
        if _bindings_available and self._neuromod_ptr:
            return self._neuromod_ptr.initialize(brain._brain_ptr if brain._brain_ptr else None)
        return True
    
    def get_level(self) -> float:
        """Get current neuromodulation level."""
        if _bindings_available and self._neuromod_ptr:
            return self._neuromod_ptr.getLevel()
        return self.level
    
    def update(self, dt: float):
        """Update neuromodulator level."""
        if _bindings_available and self._neuromod_ptr:
            return self._neuromod_ptr.update(dt)
        
        # Python fallback: level decays naturally
        self.level *= 0.95  # Natural decay
        if self.level < 0.01:
            self.level = 0.0
    
    def activate(self, strength: float = 1.0):
        """Activate neuromodulator."""
        if _bindings_available and self._neuromod_ptr:
            return self._neuromod_ptr.activate(strength)
        self.level = min(1.0, self.level + strength * 0.1)
    
    def reset(self):
        """Reset neuromodulator to initial state."""
        if _bindings_available and self._neuromod_ptr:
            return self._neuromod_ptr.reset()
        self.level = 0.0
    
    def is_active(self) -> bool:
        """Check if neuromodulator is active."""
        return self.get_level() > 0.01


class Dopamine(Neuromodulator):
    """Dopamine neuromodulator for reward and reinforcement."""
    
    def __init__(self):
        super().__init__()
        self.predicted_reward = 0.0
        self.actual_reward = 0.0
        self.error = 0.0
    
    def update(self, reward: float, predicted_reward: float, dt: float):
        """Update dopamine based on reward prediction error."""
        self.actual_reward = reward
        self.predicted_reward = predicted_reward
        self.error = reward - predicted_reward
        
        # Phasic response to prediction error
        phasic_response = self.error * 0.1  # Scale factor
        self.level = max(0.0, self.level + phasic_response)
        
        # Decay
        super().update(dt)
    
    def get_reward_modulation(self) -> float:
        """Get reward-based modulation factor."""
        # Convert dopamine level to modulation factor
        return min(1.0, self.get_level() * 2.0)


class Curiosity(Neuromodulator):
    """Curiosity drive for exploration motivation."""
    
    def __init__(self):
        super().__init__()
        self.novelty = 0.0
        self.prediction_error = 0.0
        self.novelty_weight = 0.5
        self.error_weight = 0.5
    
    def update(self, novelty: float, prediction_error: float, dt: float):
        """Update curiosity based on novelty and prediction error."""
        self.novelty = novelty
        self.prediction_error = prediction_error
        
        # Combine novelty and prediction error
        curiosity_signal = (novelty * self.novelty_weight + 
                          prediction_error * self.error_weight)
        
        # Boost level with curiosity signal
        self.level = min(1.0, self.level + curiosity_signal * 0.05)
        
        # Decay
        super().update(dt)
    
    def get_exploration_drive(self) -> float:
        """Get exploration drive factor."""
        return min(1.0, self.get_level() * 1.5)


class Novelty(Neuromodulator):
    """Novelty detection for unexpected events."""
    
    def __init__(self):
        super().__init__()
        self.history = []
        self.window_size = 10
        self.threshold = 0.5
    
    def detect_novelty(self, input_data: Any) -> float:
        """Detect novelty in input data."""
        # Simple novelty detection based on variance
        if hasattr(input_data, 'get_data'):
            data = input_data.getData()
            if len(data) > 1:
                variance = np.var(data)
                self.history.append(variance)
                if len(self.history) > self.window_size:
                    self.history.pop(0)
                
                if len(self.history) >= self.window_size:
                    recent_variance = np.mean(self.history)
                    novelty_score = min(1.0, recent_variance / self.threshold)
                    self.activate(novelty_score)
                    return novelty_score
        
        return 0.0
    
    def update(self, dt: float):
        """Update novelty detection."""
        # Decay gradually
        self.level *= 0.98
        if self.level < 0.01:
            self.level = 0.0


class PredictionError(Neuromodulator):
    """Prediction error signal for learning."""
    
    def __init__(self):
        super().__init__()
        self.predicted = 0.0
        self.actual = 0.0
        self.error = 0.0
    
    def update(self, predicted: float, actual: float, dt: float):
        """Update prediction error."""
        self.predicted = predicted
        self.actual = actual
        self.error = actual - predicted
        
        # Error magnitude drives neuromodulator level
        error_magnitude = min(1.0, abs(self.error) * 10.0)
        if self.error > 0:
            # Positive error (better than expected) excites
            self.level = min(1.0, self.level + error_magnitude * 0.1)
        else:
            # Negative error (worse than expected) inhibits
            self.level = max(0.0, self.level - error_magnitude * 0.05)
        
        # Decay
        super().update(dt)
    
    def get_error_signal(self) -> float:
        """Get the prediction error signal."""
        return self.error


class NeuromodulationSystem:
    """System that coordinates all neuromodulators."""
    
    def __init__(self):
        self.dopamine = Dopamine()
        self.curiosity = Curiosity()
        self.novelty = Novelty()
        self.prediction_error = PredictionError()
        self.brain = None
        
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                pass
    
    def initialize(self, brain):
        """Initialize neuromodulation system."""
        self.brain = brain
        self.dopamine.initialize(brain)
        self.curiosity.initialize(brain)
        self.novelty.initialize(brain)
        self.prediction_error.initialize(brain)
        
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                pass
    
    def update(self, sensory_input: Optional[SensoryInput] = None, 
               reward: float = 0.0, predicted_reward: float = 0.0, 
               dt: float = 0.1):
        """Update all neuromodulators."""
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                pass
        
        # Update each neuromodulator
        self.dopamine.update(reward, predicted_reward, dt)
        self.curiosity.update(0.0, 0.0, dt)  # Would be set by novelty and prediction error
        
        if sensory_input:
            novelty = self.novelty.detect_novelty(sensory_input)
            self.curiosity.update(novelty, 0.0, dt)
        
        self.prediction_error.update(0.0, 0.0, dt)  # Would be set by prediction system
    
    def get_combined_modulation(self) -> float:
        """Get combined neuromodulation level."""
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                pass
        
        # Weighted combination of all neuromodulators
        levels = [
            self.dopamine.get_level(),
            self.curiosity.get_level(),
            self.novelty.get_level(),
            self.prediction_error.get_level()
        ]
        return sum(levels) / len(levels) if levels else 0.0
    
    def get_modulation_factors(self) -> Dict[str, float]:
        """Get modulation factors from each system."""
        return {
            'dopamine': self.dopamine.get_reward_modulation(),
            'curiosity': self.curiosity.get_exploration_drive(),
            'novelty': self.novelty.get_level(),
            'prediction_error': self.prediction_error.get_error_signal()
        }
    
    def reset(self):
        """Reset all neuromodulators."""
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                pass
        
        self.dopamine.reset()
        self.curiosity.reset()
        self.novelty.reset()
        self.prediction_error.reset()


# Export public interface
__all__ = [
    'Neuromodulator',
    'Dopamine',
    'Curiosity',
    'Novelty',
    'PredictionError',
    'NeuromodulationSystem',
]
