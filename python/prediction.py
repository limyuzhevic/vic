"""
Prediction system for NLM - predictive coding and error computation.

This module provides the prediction system that enables predictive coding
and error minimization in the NLM artificial brain system.
"""

import sys
from typing import List, Dict, Any

__all__ = [
    'PredictionSystem',
]


class PredictionSystem:
    """Prediction system for sensory prediction and error computation.
    
    The prediction system enables predictive coding by predicting future
    sensory states and computing prediction errors. It supports learning
    through error minimization and helps the brain anticipate future input.
    
    Key features:
    - Predictive coding of sensory states
    - Prediction error computation
    - Confidence-based uncertainty estimation
    - Learning through prediction error minimization
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> prediction_system = brain.getPredictionSystem()
        >>> if prediction_system:
        ...     error = prediction_system.getPredictionError()
        ...     confidence = prediction_system.getConfidence()
        ...     print(f"Error: {error:.3f}, Confidence: {confidence:.3f}")
        ... 
        ...     # Make predictions
        ...     prediction = prediction_system.predictNextState(current_state)
        ... 
        ...     # Update predictions with actual input
        ...     prediction_system.updatePredictions(prediction, actual_state)
    """
    
    def __init__(self, prediction_system_ptr=None):
        self._prediction_system = prediction_system_ptr
    
    def predictNextState(self, current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Predict next sensory state.
        
        Args:
            current_state: Current sensory state data.
            
        Returns:
            Dict[str, Any]: Predicted next sensory state.
        """
        if self._prediction_system:
            return self._prediction_system.predictNextState(current_state)
        return {}
    
    def updatePredictions(self, predicted: Dict[str, Any], actual: Dict[str, Any]) -> bool:
        """Update predictions based on actual observation.
        
        Args:
            predicted: Predicted sensory state.
            actual: Actual sensory state.
            
        Returns:
            bool: True if predictions updated successfully.
        """
        if self._prediction_system:
            return self._prediction_system.updatePredictions(predicted, actual)
        return False
    
    def updatePredictionError(self, error: float) -> bool:
        """Update prediction error.
        
        Args:
            error: Prediction error value.
            
        Returns:
            bool: True if error updated successfully.
        """
        if self._prediction_system:
            return self._prediction_system.updatePredictionError(error)
        return False
    
    def updateConfidence(self, change: float = 0.0f) -> bool:
        """Update prediction confidence.
        
        Args:
            change: Confidence change (positive increases, negative decreases).
            
        Returns:
            bool: True if confidence updated successfully.
        """
        if self._prediction_system:
            return self._prediction_system.updateConfidence(change)
        return False
    
    def getPredictionError(self) -> float:
        """Get current prediction error.
        
        Returns:
            float: Prediction error (lower is better).
        """
        if self._prediction_system:
            return self._prediction_system.getPredictionError()
        return 0.0
    
    def getConfidence(self) -> float:
        """Get prediction confidence.
        
        Returns:
            float: Confidence level (0.0 to 1.0, higher is more confident).
        """
        if self._prediction_system:
            return self._prediction_system.getConfidence()
        return 0.0
    
    def getErrorHistory(self) -> List[float]:
        """Get prediction error history.
        
        Returns:
            List[float]: List of past prediction errors.
        """
        if self._prediction_system:
            return self._prediction_system.getErrorHistory()
        return []
    
    def clearHistory(self) -> bool:
        """Clear prediction error history.
        
        Returns:
            bool: True if history cleared successfully.
        """
        if self._prediction_system:
            return self._prediction_system.clearHistory()
        return False
    
    def train(self, observation: Dict[str, Any]) -> bool:
        """Train prediction model with observation.
        
        Args:
            observation: Observed sensory data.
            
        Returns:
            bool: True if trained successfully.
        """
        if self._prediction_system:
            return self._prediction_system.train(observation)
        return False
    
    def __repr__(self) -> str:
        if self._prediction_system:
            error = self.getPredictionError()
            confidence = self.getConfidence()
            return f"<PredictionSystem: error={error:.3f}, confidence={confidence:.3f}>"
        return "<PredictionSystem: uninitialized>"
