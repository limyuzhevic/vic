"""
Enhanced Python interface for NLM neuromodulation systems
"""

from typing import List, Dict, Any, Optional, Tuple
from .exceptions import NLMError, NLMNeuromodulationError

class Neuromodulator:
    """Base class for neuromodulators"""
    
    def __init__(self, name: str):
        self.name = name
        self.level = 0.0
        self.plasticity_factor = 1.0
        self.history = []
        
    def update(self, timestep: float) -> None:
        """Update neuromodulator level"""
        
    def set_level(self, level: float) -> None:
        """Set neuromodulator level"""
        self.level = max(-1.0, min(1.0, level))  # Clamp to [-1, 1]
        
    def get_level(self) -> float:
        """Get neuromodulator level"""
        return self.level
        
    def get_plasticity_factor(self) -> float:
        """Get plasticity modulation factor"""
        return self.plasticity_factor
        
    def set_plasticity_factor(self, factor: float) -> None:
        """Set plasticity modulation factor"""
        self.plasticity_factor = max(0.1, min(2.0, factor))  # Clamp to reasonable range
        
    def get_history(self, limit: Optional[int] = None) -> List[Tuple]:
        """Get neuromodulator history"""
        if limit is None:
            return self.history.copy()
        return self.history[-limit:]
        
    def __repr__(self):
        return f"<{self.name}: level={self.level:.3f}, plasticity_factor={self.plasticity_factor:.3f}>"

class Dopamine(Neuromodulator):
    """Dopamine neuromodulator for reward processing"""
    
    def __init__(self):
        super().__init__("Dopamine")
        self.prediction_error = 0.0
        self.reward_history = []
        
    def update(self, timestep: float, prediction_error: float) -> None:
        """Update dopamine level based on prediction error"""
        self.prediction_error = prediction_error
        
        # Dopamine level based on prediction error
        # Positive prediction error (better than expected) increases dopamine
        self.level = prediction_error
        self.plasticity_factor = 0.5 + 0.5 * prediction_error  # Modulate plasticity
        
        self.history.append(('update', timestep, prediction_error, self.level, self.plasticity_factor))
        
    def get_prediction_error(self) -> float:
        """Get prediction error"""
        return self.prediction_error
        
    def __repr__(self):
        return f"<Dopamine: level={self.level:.3f}, error={self.prediction_error:.3f}, plasticity_factor={self.plasticity_factor:.3f}>"

class Curiosity(Neuromodulator):
    """Curiosity neuromodulator for exploration"""
    
    def __init__(self):
        super().__init__("Curiosity")
        self.novelty = 0.0
        self.exploration_drive = 0.0
        
    def update(self, timestep: float, novelty: float) -> None:
        """Update curiosity level based on novelty"""
        self.novelty = novelty
        
        # Curiosity increases with novelty but decays over time
        self.level = novelty * 0.5  # Scale factor
        self.exploration_drive = min(1.0, novelty * 2.0)  # Higher curiosity = more exploration
        
        self.history.append(('update', timestep, novelty, self.level, self.exploration_drive))
        
    def get_novelty(self) -> float:
        """Get novelty level"""
        return self.novelty
        
    def get_exploration_drive(self) -> float:
        """Get exploration drive"""
        return self.exploration_drive
        
    def __repr__(self):
        return f"<Curiosity: level={self.level:.3f}, novelty={self.novelty:.3f}, exploration={self.exploration_drive:.3f}>"

class Novelty(Neuromodulator):
    """Novelty detection system"""
    
    def __init__(self):
        super().__init__("Novelty")
        self.similarity_threshold = 0.1
        self.similarity_history = []
        
    def update(self, timestep: float, current_pattern: List[float], 
               reference_pattern: Optional[List[float]] = None) -> None:
        """Update novelty detection"""
        if reference_pattern:
            # Calculate pattern similarity
            distance = self._pattern_distance(current_pattern, reference_pattern)
            similarity = 1.0 - min(1.0, distance)
            self.level = 1.0 - similarity  # Novelty = 1 - similarity
            self.similarity_history.append(similarity)
        else:
            # Default novelty
            self.level = 0.5
            self.similarity_history.append(0.5)
        
        self.history.append(('update', timestep, current_pattern, reference_pattern, self.level))
        
    def _pattern_distance(self, pattern1: List[float], pattern2: List[float]) -> float:
        """Calculate distance between two patterns"""
        if len(pattern1) != len(pattern2):
            return float('inf')
            
        distance = 0.0
        for a, b in zip(pattern1, pattern2):
            distance += (a - b) ** 2
        return distance ** 0.5
        
    def get_novelty_level(self) -> float:
        """Get current novelty level"""
        return self.level
        
    def get_similarity(self) -> float:
        """Get last similarity value"""
        if self.similarity_history:
            return self.similarity_history[-1]
        return 0.0
        
    def __repr__(self):
        return f"<Novelty: level={self.level:.3f}, similarity={self.get_similarity():.3f}>"

class PredictionError(Neuromodulator):
    """Prediction error neuromodulator"""
    
    def __init__(self):
        super().__init__("PredictionError")
        self.expected_value = 0.0
        self.actual_value = 0.0
        
    def update(self, timestep: float, expected: float, actual: float) -> None:
        """Update prediction error"""
        self.expected_value = expected
        self.actual_value = actual
        
        # Prediction error = actual - expected
        self.level = actual - expected
        self.plasticity_factor = 0.5 + 0.5 * abs(self.level)
        
        self.history.append(('update', timestep, expected, actual, self.level, self.plasticity_factor))
        
    def get_expected_value(self) -> float:
        """Get expected value"""
        return self.expected_value
        
    def get_actual_value(self) -> float:
        """Get actual value"""
        return self.actual_value
        
    def get_absolute_error(self) -> float:
        """Get absolute prediction error"""
        return abs(self.level)
        
    def __repr__(self):
        return f"<PredictionError: level={self.level:.3f}, expected={self.expected_value:.3f}, actual={self.actual_value:.3f}>"
