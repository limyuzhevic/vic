"""STDP (Spike-Timing-Dependent Plasticity) rule."""

from typing import Optional


class STDP:
    """Spike-Timing-Dependent Plasticity rule."""
    
    def __init__(self, learning_rate: float = 0.001, tau_plus: float = 20.0, tau_minus: float = 20.0):
        """
        Initialize STDP rule.
        
        Args:
            learning_rate: STDP learning rate
            tau_plus: Potentiation time constant
            tau_minus: Depression time constant
        """
        self.learning_rate = learning_rate
        self.tau_plus = tau_plus
        self.tau_minus = tau_minus
        
    def get_learning_rate(self) -> float:
        """Get learning rate."""
        return self.learning_rate
        
    def set_learning_rate(self, rate: float):
        """Set learning rate."""
        self.learning_rate = rate
        
    def get_tau_plus(self) -> float:
        """Get potentiation time constant."""
        return self.tau_plus
        
    def set_tau_plus(self, tau: float):
        """Set potentiation time constant."""
        self.tau_plus = tau
        
    def get_tau_minus(self) -> float:
        """Get depression time constant."""
        return self.tau_minus
        
    def set_tau_minus(self, tau: float):
        """Set depression time constant."""
        self.tau_minus = tau