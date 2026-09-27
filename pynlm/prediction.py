from dataclasses import dataclass, field
"""
Prediction System - Forward Models and Generative Models
=====================================================

Implementation of predictive processing and forward modeling:
- Sensory prediction and error computation
- Forward models and generative models
- Prediction error minimization
- Confidence tracking
- Prediction-based learning
"""

from typing import List, Dict, Any, Optional
import math
import numpy as np
@dataclass
class PredictionState:
    """Prediction system state."""
    
    # Current prediction
    current_prediction: List[float] = field(default_factory=list)
    
    # Prediction error
    prediction_error: float = 0.0
    
    # Confidence
    confidence: float = 1.0
    
    # History
    prediction_history: List[List[float]] = field(default_factory=list)
    error_history: List[float] = field(default_factory=list)
class PredictionSystem:
    """
    Prediction system for sensory prediction and error computation.
    
    Implements predictive processing using forward models:
    - Generate predictions of sensory input
    - Compute prediction errors
    - Update internal models based on errors
    - Minimize prediction errors over time
    
    Attributes:
        prediction_window (int): Prediction window size
        error_weight (float): Weight for prediction error
        learning_rate (float): Learning rate for model updates
        confidence_decay (float): Confidence decay rate
    """
    
    def __init__(self, prediction_window: int = 10,
                 error_weight: float = 0.1,
                 learning_rate: float = 0.01,
                 confidence_decay: float = 0.95):
        """
        Initialize prediction system.
        
        Args:
            prediction_window (int): Prediction window size
            error_weight (float): Weight for prediction error
            learning_rate (float): Learning rate for model updates
            confidence_decay (float): Confidence decay rate
        """
        self.prediction_window = prediction_window
        self.error_weight = error_weight
        self.learning_rate = learning_rate
        self.confidence_decay = confidence_decay
        
        self.state = PredictionState()
        self.is_active = False
        
    def initialize(self):
        """Initialize prediction system."""
        self.state = PredictionState()
        self.is_active = False
        
    def update(self, dt: float) -> Dict[str, Any]:
        """
        Update prediction system.
        
        Args:
            dt (float): Time step duration
            
        Returns:
            Dict[str, Any]: Prediction system statistics
        """
        # Decay confidence
        self.state.confidence *= self.confidence_decay
        
        # Limit history length
        max_history = 1000
        if len(self.state.prediction_history) > max_history:
            self.state.prediction_history.pop(0)
            self.state.error_history.pop(0)
            
        return {
            "prediction_error": self.state.prediction_error,
            "confidence": self.state.confidence,
            "prediction_active": self.is_active,
            "prediction_history_length": len(self.state.prediction_history),
            "error_history_length": len(self.state.error_history)
        }
        
    def process_sensory_input(self, input_data: List[float]):
        """
        Process sensory input and update prediction.
        
        Args:
            input_data (List[float]): Sensory input data
        """
        self.is_active = True
        
        # Generate prediction
        prediction = self._generate_prediction(input_data)
        self.state.current_prediction = prediction
        
        # Compute prediction error
        error = self._compute_prediction_error(input_data, prediction)
        self.state.prediction_error = error
        self.state.error_history.append(error)
        
        # Update confidence based on error
        if error > 0.5:
            self.state.confidence *= 0.9
        else:
            self.state.confidence = min(1.0, self.state.confidence * 1.1)
            
        # Update prediction history
        self.state.prediction_history.append(prediction)
        if len(self.state.prediction_history) > self.prediction_window:
            self.state.prediction_history.pop(0)
            
    def _generate_prediction(self, input_data: List[float]) -> List[float]:
        """
        Generate prediction based on input data.
        
        Args:
            input_data (List[float]): Input data
            
        Returns:
            List[float]: Predicted data
        """
        # Simple averaging of recent predictions
        if not self.state.prediction_history:
            return input_data.copy()
            
        recent_prediction = self.state.prediction_history[-1]
        if len(recent_prediction) != len(input_data):
            return input_data.copy()
            
        # Add small noise for exploration
        prediction = recent_prediction.copy()
        for i in range(len(prediction)):
            prediction[i] *= (1.0 + (np.random.random() - 0.5) * 0.1)
            
        return prediction
        
    def _compute_prediction_error(self, actual: List[float], 
                                 predicted: List[float]) -> float:
        """
        Compute prediction error between actual and predicted data.
        
        Args:
            actual (List[float]): Actual data
            predicted (List[float]): Predicted data
            
        Returns:
            float: Prediction error
        """
        if len(actual) != len(predicted):
            return 1.0
            
        # Mean squared error
        mse = sum((a - p) ** 2 for a, p in zip(actual, predicted)) / len(actual)
        
        return math.sqrt(mse) if mse > 0 else 0.0
        
    def generate_prediction(self, state: List[float]) -> List[float]:
        """
        Generate prediction from current state.
        
        Args:
            state (List[float]): Current state
            
        Returns:
            List[float]: Predicted next state
        """
        # Simple forward model: predict next state
        prediction = state.copy()
        for i in range(len(prediction)):
            prediction[i] *= 0.95  # Decay model
            prediction[i] += np.random.random() * 0.1  # Add noise
            
        return prediction
        
    def predict_next_state(self, current_state: List[float], dt: float) -> List[float]:
        """
        Predict next state based on current state.
        
        Args:
            current_state (List[float]): Current state
            dt (float): Time step duration
            
        Returns:
            List[float]: Predicted next state
        """
        return self.generate_prediction(current_state)
        
    def get_prediction_error(self) -> float:
        """Get current prediction error."""
        return self.state.prediction_error
        
    def get_confidence(self) -> float:
        """Get current prediction confidence."""
        return self.state.confidence
        
    def is_active(self) -> bool:
        """Check if prediction system is active."""
        return self.is_active
        
    def reset_prediction_state(self):
        """Reset prediction state."""
        self.state = PredictionState()
        self.is_active = False
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get prediction system state.
        
        Returns:
            Dict[str, Any]: Prediction system state
        """
        return {
            "current_prediction": self.state.current_prediction,
            "prediction_error": self.state.prediction_error,
            "confidence": self.state.confidence,
            "prediction_history": self.state.prediction_history,
            "error_history": self.state.error_history,
            "prediction_window": self.prediction_window,
            "error_weight": self.error_weight,
            "learning_rate": self.learning_rate,
            "confidence_decay": self.confidence_decay,
            "is_active": self.is_active
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set prediction system state.
        
        Args:
            state (Dict[str, Any]): Prediction system state
        """
        self.state.current_prediction = state.get("current_prediction", 
                                                self.state.current_prediction)
        self.state.prediction_error = state.get("prediction_error", 
                                              self.state.prediction_error)
        self.state.confidence = state.get("confidence", self.state.confidence)
        self.state.prediction_history = state.get("prediction_history", 
                                                self.state.prediction_history)
        self.state.error_history = state.get("error_history", 
                                          self.state.error_history)
        self.prediction_window = state.get("prediction_window", 
                                         self.prediction_window)
        self.error_weight = state.get("error_weight", self.error_weight)
        self.learning_rate = state.get("learning_rate", self.learning_rate)
        self.confidence_decay = state.get("confidence_decay", 
                                        self.confidence_decay)
        self.is_active = state.get("is_active", self.is_active)
        
    def reset(self):
        """Reset prediction system."""
        self.state = PredictionState()
        self.is_active = False
        
    def __repr__(self) -> str:
        return f"<PredictionSystem error={self.state.prediction_error:.3f} confidence={self.state.confidence:.3f}>"
