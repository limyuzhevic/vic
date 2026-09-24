"""
Neuromodulation systems for NLM - dopamine, curiosity, novelty, prediction error.

This module provides the neuromodulation systems that regulate neural
excitability and plasticity in the NLM artificial brain system.
"""

import sys
from typing import List, Dict, Any

__all__ = [
    'Dopamine',
    'Curiosity',
    'Novelty',
    'PredictionError',
]


class Dopamine:
    """Dopamine neuromodulator for reward and reinforcement learning.
    
    Dopamine modulates neural excitability and plasticity based on
    reward prediction errors. It enhances learning when rewards are
    better than expected and suppresses learning when rewards are worse
    than expected.
    
    Key features:
    - Reward prediction error signaling
    - Plasticity rate modulation
    - Neural excitability modulation
    - Reinforcement learning support
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> dopamine = brain.getDopamine()
        >>> if dopamine:
        ...     level = dopamine.getLevel()
        ...     print(f"Dopamine level: {level:.3f}")
        ...     
        ...     # Apply reward modulation
        ...     brain.applyRewardModulation(reward, predicted_reward)
    """
    
    def __init__(self, dopamine_ptr=None):
        self._dopamine = dopamine_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize dopamine system.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._dopamine:
            return self._dopamine.initialize(brain_ptr)
        return False
    
    def getLevel(self) -> float:
        """Get dopamine level.
        
        Returns:
            float: Dopamine level (0.0 to 1.0).
        """
        if self._dopamine:
            return self._dopamine.getLevel()
        return 0.0
    
    def getPlasticityFactor(self) -> float:
        """Get dopamine's effect on plasticity.
        
        Returns:
            float: Plasticity factor (1.0 = normal, >1.0 = enhanced, <1.0 = reduced).
        """
        if self._dopamine:
            return self._dopamine.getPlasticityFactor()
        return 1.0
    
    def setLearningRate(self, rate: float) -> bool:
        """Set dopamine learning rate.
        
        Args:
            rate: Learning rate (0.0 to 1.0).
            
        Returns:
            bool: True if learning rate set successfully.
        """
        if self._dopamine:
            return self._dopamine.setLearningRate(rate)
        return False
    
    def update(self, dt: float) -> bool:
        """Update dopamine dynamics.
        
        Args:
            dt: Time step duration.
            
        Returns:
            bool: True if update successful.
        """
        if self._dopamine:
            return self._dopamine.update(dt)
        return False
    
    def __repr__(self) -> str:
        if self._dopamine:
            level = self.getLevel()
            return f"<Dopamine: level={level:.3f}>"
        return "<Dopamine: uninitialized>"


class Curiosity:
    """Curiosity neuromodulator for exploration motivation.
    
    Curiosity drives exploration by generating motivation to seek novel
    and uncertain information. It increases when prediction errors are
    large or when novel stimuli are encountered.
    
    Key features:
    - Exploration motivation
    - Novelty detection
    - Information seeking behavior
    - Reward prediction error modulation
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> curiosity = brain.getCuriosity()
        >>> if curiosity:
        ...     level = curiosity.getLevel()
        ...     print(f"Curiosity level: {level:.3f}")
        ...     
        ...     # Enable curiosity
        ...     curiosity.setEnabled(True)
    """
    
    def __init__(self, curiosity_ptr=None):
        self._curiosity = curiosity_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize curiosity system.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._curiosity:
            return self._curiosity.initialize(brain_ptr)
        return False
    
    def getLevel(self) -> float:
        """Get curiosity level.
        
        Returns:
            float: Curiosity level (0.0 to 1.0).
        """
        if self._curiosity:
            return self._curiosity.getLevel()
        return 0.0
    
    def setEnabled(self, enabled: bool) -> bool:
        """Enable or disable curiosity.
        
        Args:
            enabled: True to enable curiosity, False to disable.
            
        Returns:
            bool: True if enabled status changed successfully.
        """
        if self._curiosity:
            return self._curiosity.setEnabled(enabled)
        return False
    
    def update(self, dt: float) -> bool:
        """Update curiosity dynamics.
        
        Args:
            dt: Time step duration.
            
        Returns:
            bool: True if update successful.
        """
        if self._curiosity:
            return self._curiosity.update(dt)
        return False
    
    def __repr__(self) -> str:
        if self._curiosity:
            level = self.getLevel()
            return f"<Curiosity: level={level:.3f}>"
        return "<Curiosity: uninitialized>"


class Novelty:
    """Novelty detection system for identifying new information.
    
    Novelty detection identifies novel or unexpected sensory input by
    comparing incoming information with predictions. It drives exploration
    by signaling when new information is encountered.
    
    Key features:
    - Novelty detection and signaling
    - Uncertainty estimation
    - Change detection
    - Exploration drive
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> novelty = brain.getNovelty()
        >>> if novelty:
        ...     level = novelty.getLevel()
        ...     print(f"Novelty level: {level:.3f}")
        ...     
        ...     # Update novelty detection
        ...     novelty.update(dt)
    """
    
    def __init__(self, novelty_ptr=None):
        self._novelty = novelty_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize novelty detection.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._novelty:
            return self._novelty.initialize(brain_ptr)
        return False
    
    def getLevel(self) -> float:
        """Get novelty level.
        
        Returns:
            float: Novelty level (0.0 to 1.0).
        """
        if self._novelty:
            return self._novelty.getLevel()
        return 0.0
    
    def update(self, dt: float) -> bool:
        """Update novelty detection.
        
        Args:
            dt: Time step duration.
            
        Returns:
            bool: True if update successful.
        """
        if self._novelty:
            return self._novelty.update(dt)
        return False
    
    def __repr__(self) -> str:
        if self._novelty:
            level = self.getLevel()
            return f"<Novelty: level={level:.3f}>"
        return "<Novelty: uninitialized>"


class PredictionError:
    """Prediction error system for learning signal.
    
    Prediction error represents the difference between predicted and
    actual sensory input. It serves as a learning signal that drives
    reinforcement learning and plasticity.
    
    Key features:
    - Prediction error computation
    - Learning signal generation
    - Credit assignment
    - Value learning
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> pred_error = brain.getPredictionErrorSignal()
        >>> if pred_error:
        ...     error = pred_error.getLevel()
        ...     print(f"Prediction error: {error:.3f}")
        ...     
        ...     # Update prediction error
        ...     pred_error.update(error, reward)
    """
    
    def __init__(self, pred_error_ptr=None):
        self._pred_error = pred_error_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize prediction error system.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._pred_error:
            return self._pred_error.initialize(brain_ptr)
        return False
    
    def getLevel(self) -> float:
        """Get prediction error level.
        
        Returns:
            float: Prediction error level (can be positive or negative).
        """
        if self._pred_error:
            return self._pred_error.getLevel()
        return 0.0
    
    def getLearningRate(self) -> float:
        """Get prediction error learning rate.
        
        Returns:
            float: Learning rate (0.0 to 1.0).
        """
        if self._pred_error:
            return self._pred_error.getLearningRate()
        return 0.0
    
    def updatePredictionError(self, error: float) -> bool:
        """Update prediction error.
        
        Args:
            error: New prediction error value.
            
        Returns:
            bool: True if error updated successfully.
        """
        if self._pred_error:
            return self._pred_error.updatePredictionError(error)
        return False
    
    def update(self, error: float, reward: float) -> bool:
        """Update prediction error with reward.
        
        Args:
            error: Prediction error.
            reward: Received reward.
            
        Returns:
            bool: True if error updated successfully.
        """
        if self._pred_error:
            return self._pred_error.update(error, reward)
        return False
    
    def __repr__(self) -> str:
        if self._pred_error:
            level = self.getLevel()
            return f"<PredictionError: level={level:.3f}>"
        return "<PredictionError: uninitialized>"
