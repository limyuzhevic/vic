"""
Enhanced Python interface for NLM prediction system
"""

from typing import List, Dict, Any, Optional, Tuple
from .exceptions import NLMError

class NeuralPrediction:
    """Neural prediction model"""
    
    def __init__(self, learning_rate: float = 0.1):
        self.prediction = 0.0
        self.target = 0.0
        self.error = 0.0
        self.confidence = 1.0
        self.accuracy = 0.0
        self.learning_rate = learning_rate
        self.history = []  # List of prediction tuples
        
    def predict(self, input_data: List[float]) -> float:
        """Make prediction based on input data"""
        # Simple linear prediction for demonstration
        self.prediction = sum(input_data) / len(input_data) if input_data else 0.0
        self.history.append(('predict', input_data.copy(), self.prediction))
        return self.prediction
        
    def set_target(self, target: float) -> None:
        """Set target value for learning"""
        self.target = target
        
    def update(self, target: float) -> float:
        """Update prediction based on target (learning)"""
        self.set_target(target)
        
        # Calculate error
        self.error = target - self.prediction
        
        # Update prediction
        self.prediction += self.learning_rate * self.error
        
        # Update confidence and accuracy
        self.confidence = 1.0 / (1.0 + abs(self.error))
        self.accuracy = max(0.0, 1.0 - abs(self.error))
        
        self.history.append(('update', target, self.prediction, self.error))
        return self.error
        
    def get_prediction(self) -> float:
        """Get current prediction"""
        return self.prediction
        
    def get_error(self) -> float:
        """Get prediction error"""
        return self.error
        
    def get_confidence(self) -> float:
        """Get prediction confidence"""
        return self.confidence
        
    def get_accuracy(self) -> float:
        """Get prediction accuracy"""
        return self.accuracy
        
    def get_history(self, limit: Optional[int] = None) -> List[Tuple]:
        """Get prediction history"""
        if limit is None:
            return self.history.copy()
        return self.history[-limit:]
        
    def reset(self) -> None:
        """Reset prediction model"""
        self.prediction = 0.0
        self.target = 0.0
        self.error = 0.0
        self.confidence = 1.0
        self.accuracy = 0.0
        self.history.clear()
        
    def __repr__(self):
        return f"<NeuralPrediction: pred={self.prediction:.3f}, error={self.error:.3f}, conf={self.confidence:.3f}>"

class PredictionSystem:
    """Prediction system for NLM"""
    
    def __init__(self, num_predictors: int = 10):
        self.predictors = [NeuralPrediction() for _ in range(num_predictors)]
        self.sensory_predictions = []
        self.motor_predictions = []
        self.error_correction_enabled = True
        self.prediction_horizon = 100  # Number of steps to predict ahead
        
    def add_sensory_prediction(self, modality: str, prediction: NeuralPrediction) -> None:
        """Add sensory modality prediction"""
        self.sensory_predictions.append((modality, prediction))
        
    def add_motor_prediction(self, action_type: str, prediction: NeuralPrediction) -> None:
        """Add motor action prediction"""
        self.motor_predictions.append((action_type, prediction))
        
    def predict_sensory(self, sensory_data: List[float]) -> List[float]:
        """Predict sensory outcomes"""
        predictions = []
        for modality, predictor in self.sensory_predictions:
            pred = predictor.predict(sensory_data)
            predictions.append(pred)
        return predictions
        
    def predict_motor(self, current_state: Dict[str, Any]) -> List[float]:
        """Predict motor outcomes"""
        predictions = []
        for action_type, predictor in self.motor_predictions:
            pred = predictor.predict(list(current_state.values()))
            predictions.append(pred)
        return predictions
        
    def update_predictions(self, target_values: List[float]) -> List[float]:
        """Update predictions based on actual outcomes"""
        errors = []
        for i, target in enumerate(target_values):
            if i < len(self.predictors):
                error = self.predictors[i].update(target)
                errors.append(error)
        return errors
        
    def get_system_prediction(self) -> Dict[str, Any]:
        """Get comprehensive system prediction"""
        return {
            'num_sensory_predictions': len(self.sensory_predictions),
            'num_motor_predictions': len(self.motor_predictions),
            'avg_prediction_confidence': sum(p.get_confidence() for _, p in self.sensory_predictions) / max(1, len(self.sensory_predictions)),
            'avg_prediction_accuracy': sum(p.get_accuracy() for _, p in self.sensory_predictions) / max(1, len(self.sensory_predictions)),
            'total_predictors': len(self.predictors)
        }
        
    def reset(self) -> None:
        """Reset prediction system"""
        for predictor in self.predictors:
            predictor.reset()
        self.sensory_predictions.clear()
        self.motor_predictions.clear()
        
    def enable_error_correction(self, enable: bool) -> None:
        """Enable/disable error correction"""
        self.error_correction_enabled = enable
        
    def get_error_correction_enabled(self) -> bool:
        """Check if error correction is enabled"""
        return self.error_correction_enabled
        
    def __repr__(self):
        stats = self.get_system_prediction()
        return f"<PredictionSystem: {stats['num_sensory_predictions']} sensory, {stats['num_motor_predictions']} motor, {len(self.predictors)} predictors>"
