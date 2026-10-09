"""
Enhanced Python interface for NLM development system
"""

from typing import List, Dict, Any, Optional, Tuple
from enum import Enum
from .exceptions import NLMError, NLMDevelopmentError

class DevelopmentalStage(Enum):
    """Developmental stage enumeration"""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class DevelopmentSystem:
    """Development system for neural maturation"""
    
    def __init__(self):
        self.current_stage = DevelopmentalStage.INITIAL
        self.age = 0.0
        self.plasticity_rate = 1.0
        self.synaptogenesis_rate = 0.0001
        self.pruning_rate = 0.00001
        self.receptive_field_size = 1.0
        self.mature_connections = []
        self.development_history = []
        
    def update(self, brain, timestep: float) -> None:
        """Update development system"""
        # Simple developmental stages based on age
        if self.age < 60.0:  # Initial stage
            self.current_stage = DevelopmentalStage.INITIAL
            self.plasticity_rate = 1.0
        elif self.age < 300.0:  # Critical period
            self.current_stage = DevelopmentalStage.CRITICAL_PERIOD
            self.plasticity_rate = 0.8
        elif self.age < 900.0:  # Maturation
            self.current_stage = DevelopmentalStage.MATURATION
            self.plasticity_rate = 0.5
        else:  # Adult
            self.current_stage = DevelopmentalStage.ADULT
            self.plasticity_rate = 0.2
            
        # Update plastic rates based on developmental stage
        self._update_plastic_rates()
        
        # Record development
        self.development_history.append({
            'age': self.age,
            'stage': self.current_stage.value,
            'plasticity_rate': self.plasticity_rate,
            'synaptogenesis_rate': self.synaptogenesis_rate,
            'pruning_rate': self.pruning_rate
        })
        
    def _update_plastic_rates(self) -> None:
        """Update plasticity rates based on developmental stage"""
        stage = self.current_stage
        if stage == DevelopmentalStage.INITIAL:
            self.synaptogenesis_rate = 0.0001
            self.pruning_rate = 0.00001
        elif stage == DevelopmentalStage.CRITICAL_PERIOD:
            self.synaptogenesis_rate = 0.00005
            self.pruning_rate = 0.000005
        elif stage == DevelopmentalStage.MATURATION:
            self.synaptogenesis_rate = 0.00001
            self.pruning_rate = 0.000001
        else:  # Adult
            self.synaptogenesis_rate = 0.000001
            self.pruning_rate = 0.0000001
            
    def get_developmental_stage(self) -> str:
        """Get current developmental stage"""
        return self.current_stage.value
        
    def set_developmental_stage(self, stage: str) -> None:
        """Set developmental stage (for testing)"""
        try:
            self.current_stage = DevelopmentalStage(stage)
            self._update_plastic_rates()
        except ValueError:
            raise NLMDevelopmentError(f"Invalid developmental stage: {stage}")
        
    def get_age(self) -> float:
        """Get current age"""
        return self.age
        
    def get_plasticity_rate(self) -> float:
        """Get current plasticity rate"""
        return self.plasticity_rate
        
    def get_synaptogenesis_rate(self) -> float:
        """Get synaptogenesis rate"""
        return self.synaptogenesis_rate
        
    def get_pruning_rate(self) -> float:
        """Get pruning rate"""
        return self.pruning_rate
        
    def get_receptive_field_size(self) -> float:
        """Get receptive field size"""
        return self.receptive_field_size
        
    def increment_age(self, delta: float) -> None:
        """Increment age by delta"""
        self.age += delta
        
    def add_mature_connection(self, connection: Dict[str, Any]) -> None:
        """Add mature connection to development system"""
        self.mature_connections.append(connection)
        
    def get_mature_connections(self) -> List[Dict[str, Any]]:
        """Get mature connections"""
        return self.mature_connections.copy()
        
    def get_development_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get development history"""
        if limit is None:
            return self.development_history.copy()
        return self.development_history[-limit:]
        
    def clear(self) -> None:
        """Clear development history"""
        self.development_history.clear()
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get development system statistics"""
        if not self.development_history:
            return {
                'current_stage': self.current_stage.value,
                'age': self.age,
                'plasticity_rate': self.plasticity_rate,
                'development_events': 0,
                'mature_connections': len(self.mature_connections)
            }
            
        return {
            'current_stage': self.current_stage.value,
            'age': self.age,
            'plasticity_rate': self.plasticity_rate,
            'development_events': len(self.development_history),
            'synaptogenesis_rate': self.synaptogenesis_rate,
            'pruning_rate': self.pruning_rate,
            'mature_connections': len(self.mature_connections),
            'last_stage_change': self._get_last_stage_change_age()
        }
        
    def _get_last_stage_change_age(self) -> float:
        """Get age at last stage change"""
        for event in reversed(self.development_history):
            if 'stage' in event:
                return event['age']
        return 0.0
        
    def __repr__(self):
        stats = self.get_statistics()
        return f"<DevelopmentSystem: stage={stats['current_stage']}, age={stats['age']:.1f}, plasticity={stats['plasticity_rate']:.3f}, connections={stats['mature_connections']}>"
