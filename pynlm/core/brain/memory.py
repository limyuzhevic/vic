"""Memory structures for NLM brain simulation."""

from typing import Dict, Any


class NeuronState:
    """Memory representation of neuron state."""
    
    def __init__(self):
        self.neuron_id: int = 0
        self.membrane_potential: float = -70.0
        self.firing_state: str = "RESTING"
        self.threshold: float = -55.0
        self.refractory_remaining: float = 0.0
        self.adaptation_variable: float = 0.0
        self.last_spike_time: float = -1.0
        self.firing_rate: float = 0.0
        self.spike_history: list = []
        self.input_history: list = []
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert neuron state to dictionary."""
        return {
            'neuron_id': self.neuron_id,
            'membrane_potential': self.membrane_potential,
            'firing_state': self.firing_state,
            'threshold': self.threshold,
            'refractory_remaining': self.refractory_remaining,
            'adaptation_variable': self.adaptation_variable,
            'last_spike_time': self.last_spike_time,
            'firing_rate': self.firing_rate,
            'spike_history': self.spike_history,
            'input_history': self.input_history
        }
        
    def from_dict(self, data: Dict[str, Any]):
        """Load neuron state from dictionary."""
        for key, value in data.items():
            if hasattr(self, key):
                setattr(self, key, value)