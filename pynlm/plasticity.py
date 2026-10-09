"""
Enhanced Python interface for NLM plasticity systems
"""

from typing import List, Dict, Any, Optional, Tuple
from .exceptions import NLMError

class PlasticityRule:
    """Base class for plasticity rules"""
    
    def __init__(self, name: str):
        self.name = name
        self.enabled = True
        self.weight = 1.0
        self.history = []
        
    def update(self, synapse, pre_spike_times: List[float], 
               post_spike_times: List[float], timestep: float) -> float:
        """Update synapse weight based on spike times"""
        return 0.0
        
    def enable(self, enable: bool = True) -> None:
        """Enable/disable plasticity rule"""
        self.enabled = enable
        
    def is_enabled(self) -> bool:
        """Check if rule is enabled"""
        return self.enabled
        
    def get_weight(self) -> float:
        """Get rule weight"""
        return self.weight
        
    def set_weight(self, weight: float) -> None:
        """Set rule weight"""
        self.weight = weight
        
    def get_history(self, limit: Optional[int] = None) -> List[Any]:
        """Get rule history"""
        if limit is None:
            return self.history.copy()
        return self.history[-limit:]
        
    def __repr__(self):
        return f"<{self.name}: enabled={self.enabled}, weight={self.weight:.3f}>"

class STDP(PlasticityRule):
    """Spike-Timing Dependent Plasticity rule"""
    
    def __init__(self, ltp_weight: float = 0.01, ltd_weight: float = 0.012, 
                 tau_plus: float = 20.0, tau_minus: float = 20.0):
        super().__init__("STDP")
        self.ltp_weight = ltp_weight
        self.ltd_weight = ltd_weight
        self.tau_plus = tau_plus
        self.tau_minus = tau_minus
        
    def update(self, synapse, pre_spike_times: List[float], 
               post_spike_times: List[float], timestep: float) -> float:
        """Update synapse based on STDP"""
        if not pre_spike_times or not post_spike_times:
            return 0.0
            
        # Calculate STDP weight change
        weight_change = 0.0
        
        for post_time in post_spike_times:
            for pre_time in pre_spike_times:
                delta_t = post_time - pre_time
                if delta_t > 0:  # Post before pre (LTP)
                    weight_change += self.ltp_weight * exp(-delta_t / self.tau_plus)
                else:  # Post after pre (LTD)
                    weight_change += -self.ltd_weight * exp(delta_t / self.tau_minus)
                    
        # Apply weight change
        current_weight = synapse.get_weight()
        new_weight = current_weight + weight_change * self.weight
        
        # Clamp weight to reasonable range
        new_weight = max(-1.0, min(1.0, new_weight))
        
        self.history.append({
            'pre_spikes': len(pre_spike_times),
            'post_spikes': len(post_spike_times),
            'delta_t': post_time - pre_time if pre_spike_times and post_spike_times else 0,
            'weight_change': weight_change,
            'new_weight': new_weight
        })
        
        return weight_change
        
    def set_ltp_weight(self, weight: float) -> None:
        """Set LTP weight"""
        self.ltp_weight = weight
        
    def set_ltd_weight(self, weight: float) -> None:
        """Set LTD weight"""
        self.ltd_weight = weight
        
    def set_tau_plus(self, tau: float) -> None:
        """Set tau plus (LTP time constant)"""
        self.tau_plus = tau
        
    def set_tau_minus(self, tau: float) -> None:
        """Set tau minus (LTD time constant)"""
        self.tau_minus = tau
        
    def __repr__(self):
        return f"<STDP: ltp={self.ltp_weight:.4f}, ltd={self.ltd_weight:.4f}, tau_plus={self.tau_plus:.1f}, tau_minus={self.tau_minus:.1f}>"

class Hebbian(PlasticityRule):
    """Hebbian learning rule"""
    
    def __init__(self, learning_rate: float = 0.01):
        super().__init__("Hebbian")
        self.learning_rate = learning_rate
        
    def update(self, synapse, pre_spike_times: List[float], 
               post_spike_times: List[float], timestep: float) -> float:
        """Update synapse based on Hebbian learning"""
        if not pre_spike_times or not post_spike_times:
            return 0.0
            
        # Hebbian weight change: increase if both pre and post spiked
        weight_change = self.learning_rate * self.weight
        
        current_weight = synapse.get_weight()
        new_weight = current_weight + weight_change
        
        # Clamp weight
        new_weight = max(-1.0, min(1.0, new_weight))
        
        self.history.append({
            'pre_spikes': len(pre_spike_times),
            'post_spikes': len(post_spike_times),
            'weight_change': weight_change,
            'new_weight': new_weight
        })
        
        return weight_change
        
    def set_learning_rate(self, rate: float) -> None:
        """Set learning rate"""
        self.learning_rate = rate
        
    def __repr__(self):
        return f"<Hebbian: learning_rate={self.learning_rate:.4f}>"

class StructuralPlasticity:
    """Structural plasticity for synapse formation and elimination"""
    
    def __init__(self, synaptogenesis_rate: float = 0.0001, 
                 pruning_rate: float = 0.00001):
        self.synaptogenesis_rate = synaptogenesis_rate
        self.pruning_rate = pruning_rate
        self.synapses_to_create = []
        self.synapses_to_remove = []
        self.history = []
        
    def update(self, brain, random_generator) -> Dict[str, int]:
        """Update structural plasticity"""
        # Create new synapses
        num_to_create = int(brain.get_total_synapse_count() * self.synaptogenesis_rate)
        self.synapses_to_create = []  # Would contain synapse creation info
        
        # Remove old synapses  
        num_to_remove = int(brain.get_total_synapse_count() * self.pruning_rate)
        self.synapses_to_remove = []  # Would contain synapse removal info
        
        self.history.append({
            'synaptogenesis_events': num_to_create,
            'pruning_events': num_to_remove,
            'total_synapses_before': brain.get_total_synapse_count(),
            'total_synapses_after': brain.get_total_synapse_count() + num_to_create - num_to_remove
        })
        
        return {
            'synaptogenesis': num_to_create,
            'pruning': num_to_remove,
            'total_synapses': brain.get_total_synapse_count() + num_to_create - num_to_remove
        }
        
    def set_synaptogenesis_rate(self, rate: float) -> None:
        """Set synaptogenesis rate"""
        self.synaptogenesis_rate = rate
        
    def set_pruning_rate(self, rate: float) -> None:
        """Set pruning rate"""
        self.pruning_rate = rate
        
    def get_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get structural plasticity history"""
        if limit is None:
            return self.history.copy()
        return self.history[-limit:]
        
    def __repr__(self):
        return f"<StructuralPlasticity: synaptogenesis={self.synaptogenesis_rate:.6f}, pruning={self.pruning_rate:.6f}>"
