"""
Plasticity module - Plasticity rules for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import math
import time

from .pynlm import (
    PlasticityRule as PlasticityRuleCore, HebbianRule as HebbianRuleCore,
    STDP as STDPCore, StructuralPlasticity as StructuralPlasticityCore,
    Synapse as SynapseCore, Neuron as NeuronCore, Brain as BrainCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    SynapticWeight, FiringRate, FiringState, NeuronType, RegionId, PopulationId
)

__all__ = [
    'PlasticityRule', 'HebbianRule', 'STDP', 'StructuralPlasticity',
    'Synapse', 'Neuron', 'Brain',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'SynapticWeight', 'FiringRate', 'FiringState', 'NeuronType', 'RegionId', 'PopulationId',
]

class PlasticityRule:
    """Abstract base class for plasticity rules"""
    
    def __init__(self, enabled: bool = True):
        """Initialize plasticity rule
        
        Args:
            enabled: Whether rule is enabled
        """
        self.enabled = enabled
        self.learning_rate = 0.01
        self.timestamp = time.time()
    
    def update(self, synapse: SynapseCore, pre_spikes: List[Timestamp], 
               post_spikes: List[Timestamp], dt: TimestepDuration) -> SynapticWeight:
        """Update synaptic weights based on activity
        
        Args:
            synapse: Synapse to update
            pre_spikes: Presynaptic spike times
            post_spikes: Postsynaptic spike times
            dt: Time step
            
        Returns:
            Weight change
        """
        raise NotImplementedError("Subclasses must implement update method")
    
    def apply_weight_change(self, synapse: SynapseCore, delta: SynapticWeight):
        """Apply weight change to synapse
        
        Args:
            synapse: Synapse
            delta: Weight change
        """
        raise NotImplementedError("Subclasses must implement apply_weight_change method")
    
    def get_name(self) -> str:
        """Get rule name
        
        Returns:
            Rule name
        """
        return self.__class__.__name__
    
    def is_enabled(self) -> bool:
        """Check if rule is enabled
        
        Returns:
            True if enabled
        """
        return self.enabled
    
    def set_enabled(self, enabled: bool):
        """Set rule enabled state
        
        Args:
            enabled: Whether to enable
        """
        self.enabled = enabled
        self.timestamp = time.time()
    
    def get_learning_rate(self) -> float:
        """Get learning rate
        
        Returns:
            Learning rate
        """
        return self.learning_rate
    
    def set_learning_rate(self, rate: float):
        """Set learning rate
        
        Args:
            rate: Learning rate
        """
        if rate >= 0:
            self.learning_rate = rate
            self.timestamp = time.time()
    
    def get_timestamp(self) -> float:
        """Get last update timestamp
        
        Returns:
            Timestamp
        """
        return self.timestamp

class HebbianRule(PlasticityRule):
    """Hebbian plasticity rule: 'neurons that fire together, wire together'"""
    
    def __init__(self, learning_rate: float = 0.01, time_window: float = 10.0):
        """Initialize Hebbian rule
        
        Args:
            learning_rate: Learning rate
            time_window: Time window for spike pairing
        """
        super().__init__()
        self.learning_rate = learning_rate
        self.time_window = time_window  # ms
        self.spike_pairs = []  # List of (pre_time, post_time)
    
    def update(self, synapse: SynapseCore, pre_spikes: List[Timestamp], 
               post_spikes: List[Timestamp], dt: TimestepDuration) -> SynapticWeight:
        """Apply Hebbian learning
        
        Args:
            synapse: Synapse to update
            pre_spikes: Presynaptic spike times
            post_spikes: Postsynaptic spike times
            dt: Time step
            
        Returns:
            Weight change
        """
        if not pre_spikes or not post_spikes or not self.enabled:
            return 0.0
        
        # Count coincident spikes
        coincident_pairs = self._count_coincident_spikes(pre_spikes, post_spikes)
        
        # Calculate weight change
        weight_change = self.learning_rate * coincident_pairs
        
        # Apply weight change
        self.apply_weight_change(synapse, weight_change)
        
        # Record spike pair
        for pre_time in pre_spikes:
            for post_time in post_spikes:
                if abs(pre_time - post_time) <= self.time_window:
                    self.spike_pairs.append((pre_time, post_time))
        
        # Clean old spike pairs
        self._clean_old_spike_pairs()
        
        return weight_change
    
    def _count_coincident_spikes(self, pre_spikes: List[Timestamp], 
                                 post_spikes: List[Timestamp]) -> int:
        """Count coincident spikes
        
        Args:
            pre_spikes: Presynaptic spike times
            post_spikes: Postsynaptic spike times
            
        Returns:
            Number of coincident spike pairs
        """
        coincident = 0
        
        for pre_time in pre_spikes:
            for post_time in post_spikes:
                if abs(pre_time - post_time) <= self.time_window:
                    coincident += 1
        
        return coincident
    
    def _clean_old_spike_pairs(self):
        """Remove old spike pairs"""
        # Keep only recent spike pairs
        current_time = time.time()
        self.spike_pairs = [(pre, post) for pre, post in self.spike_pairs 
                          if (current_time - post.timestamp) < 1000.0]  # Keep last 1000 seconds
    
    def get_spike_pair_count(self) -> int:
        """Get number of recorded spike pairs
        
        Returns:
            Number of spike pairs
        """
        return len(self.spike_pairs)

class STDP(PlasticityRule):
    """Spike-Timing-Dependent Plasticity (STDP) rule"""
    
    def __init__(self, learning_rate: float = 0.01, ltp_weight: float = 0.01, 
                 ltd_weight: float = 0.012, time_constant: float = 20.0):
        """Initialize STDP rule
        
        Args:
            learning_rate: Learning rate
            ltp_weight: LTP weight
            ltd_weight: LTD weight
            time_constant: Time constant for weight changes
        """
        super().__init__()
        self.learning_rate = learning_rate
        self.ltp_weight = ltp_weight
        self.ltd_weight = ltd_weight
        self.time_constant = time_constant
        self.weight_history = []  # List of (timestamp, weight)
    
    def update(self, synapse: SynapseCore, pre_spikes: List[Timestamp], 
               post_spikes: List[Timestamp], dt: TimestepDuration) -> SynapticWeight:
        """Apply STDP learning
        
        Args:
            synapse: Synapse to update
            pre_spikes: Presynaptic spike times
            post_spikes: Postsynaptic spike times
            dt: Time step
            
        Returns:
            Weight change
        """
        if not pre_spikes or not post_spikes or not self.enabled:
            return 0.0
        
        total_weight_change = 0.0
        
        # Process presynaptic spikes
        for pre_time in pre_spikes:
            # Check if there's a recent postsynaptic spike
            recent_post_spikes = [t for t in post_spikes if t > pre_time - self.time_constant]
            
            if recent_post_spikes:
                # LTP: presynaptic spike precedes postsynaptic spike
                delta_t = min(recent_post_spikes) - pre_time
                weight_change = self.ltp_weight * math.exp(-delta_t / self.time_constant)
                self.apply_weight_change(synapse, weight_change)
                total_weight_change += weight_change
        
        # Process postsynaptic spikes
        for post_time in post_spikes:
            # Check if there's a recent presynaptic spike
            recent_pre_spikes = [t for t in pre_spikes if t > post_time - self.time_constant]
            
            if recent_pre_spikes:
                # LTD: postsynaptic spike precedes presynaptic spike
                delta_t = min(recent_pre_spikes) - post_time
                weight_change = -self.ltd_weight * math.exp(-delta_t / self.time_constant)
                self.apply_weight_change(synapse, weight_change)
                total_weight_change += weight_change
        
        # Record weight
        self.weight_history.append((time.time(), synapse.getWeight()))
        
        # Clean old weight history
        self._clean_old_weight_history()
        
        return total_weight_change
    
    def _clean_old_weight_history(self):
        """Remove old weight history"""
        # Keep only recent weight history
        current_time = time.time()
        self.weight_history = [(ts, w) for ts, w in self.weight_history 
                             if (current_time - ts) < 3600.0]  # Keep last hour
    
    def get_weight_change_history(self) -> List[Tuple[float, float]]:
        """Get weight change history
        
        Returns:
            List of (timestamp, weight) tuples
        """
        return self.weight_history.copy()
    
    def get_average_weight(self) -> float:
        """Get average weight
        
        Returns:
            Average weight
        """
        if not self.weight_history:
            return 0.0
        
        total_weight = sum(w for _, w in self.weight_history)
        return total_weight / len(self.weight_history)

class StructuralPlasticity:
    """Structural plasticity for synapse and neuron creation/removal"""
    
    def __init__(self, brain: Optional[BrainCore] = None):
        """Initialize structural plasticity
        
        Args:
            brain: Brain instance
        """
        self.brain = brain
        self.synaptogenesis_rate = 0.001
        self.pruning_rate = 0.0001
        self.threshold = 0.01
        self.timestamp = time.time()
    
    def create_synapse(self, source: NeuronId, destination: NeuronId, 
                       initial_weight: SynapticWeight = 1.0) -> Optional[SynapseId]:
        """Create new synapse
        
        Args:
            source: Source neuron
            destination: Destination neuron
            initial_weight: Initial weight
            
        Returns:
            Synapse ID or None if creation failed
        """
        if self.brain is None:
            return None
        
        try:
            # In real implementation, this would call brain.createSynapse
            # For now, return a dummy ID
            return SynapseId(0)  # Placeholder
        except Exception:
            return None
    
    def remove_synapse(self, synapse_id: SynapseId) -> bool:
        """Remove synapse
        
        Args:
            synapse_id: Synapse to remove
            
        Returns:
            True if removed successfully
        """
        if self.brain is None:
            return False
        
        try:
            # In real implementation, this would call brain.removeSynapse
            return True  # Placeholder
        except Exception:
            return False
    
    def create_neuron(self, neuron_type: NeuronType = NeuronType::Excitatory) -> Optional[NeuronId]:
        """Create new neuron
        
        Args:
            neuron_type: Type of neuron
            
        Returns:
            Neuron ID or None if creation failed
        """
        if self.brain is None:
            return None
        
        try:
            # In real implementation, this would call brain.createNeuron
            # For now, return a dummy ID
            return NeuronId(0)  # Placeholder
        except Exception:
            return None
    
    def remove_neuron(self, neuron_id: NeuronId) -> bool:
        """Remove neuron
        
        Args:
            neuron_id: Neuron to remove
            
        Returns:
            True if removed successfully
        """
        if self.brain is None:
            return False
        
        try:
            # In real implementation, this would call brain.removeNeuron
            return True  # Placeholder
        except Exception:
            return False
    
    def update(self, brain: BrainCore, rng: Any = None):
        """Update structural plasticity
        
        Args:
            brain: Brain to update
            rng: Random number generator
        """
        # Update synaptogenesis
        if rng is not None and rng.randomFloat() < self.synaptogenesis_rate:
            # Create new synapse
            pass  # Placeholder
        
        # Update pruning
        if rng is not None and rng.randomFloat() < self.pruning_rate:
            # Remove weak synapse
            pass  # Placeholder
        
        self.brain = brain
        self.timestamp = time.time()
    
    def get_synaptogenesis_rate(self) -> float:
        """Get synaptogenesis rate
        
        Returns:
            Synaptogenesis rate
        """
        return self.synaptogenesis_rate
    
    def set_synaptogenesis_rate(self, rate: float):
        """Set synaptogenesis rate
        
        Args:
            rate: Synaptogenesis rate
        """
        if rate >= 0:
            self.synaptogenesis_rate = rate
            self.timestamp = time.time()
    
    def get_pruning_rate(self) -> float:
        """Get pruning rate
        
        Returns:
            Pruning rate
        """
        return self.pruning_rate
    
    def set_pruning_rate(self, rate: float):
        """Set pruning rate
        
        Args:
            rate: Pruning rate
        """
        if rate >= 0:
            self.pruning_rate = rate
            self.timestamp = time.time()
    
    def get_threshold(self) -> float:
        """Get pruning threshold
        
        Returns:
            Threshold
        """
        return self.threshold
    
    def set_threshold(self, threshold: float):
        """Set pruning threshold
        
        Args:
            threshold: Threshold
        """
        if threshold >= 0:
            self.threshold = threshold
            self.timestamp = time.time()
    
    def get_timestamp(self) -> float:
        """Get last update timestamp
        
        Returns:
            Timestamp
        """
        return self.timestamp

# Convenience functions
def create_hebbian_rule(learning_rate: float = 0.01, time_window: float = 10.0) -> HebbianRule:
    """Create Hebbian rule
    
    Args:
        learning_rate: Learning rate
        time_window: Time window
        
    Returns:
        HebbianRule instance
    """
    return HebbianRule(learning_rate, time_window)

def create_stdp_rule(learning_rate: float = 0.01, ltp_weight: float = 0.01,
                     ltd_weight: float = 0.012, time_constant: float = 20.0) -> STDP:
    """Create STDP rule
    
    Args:
        learning_rate: Learning rate
        ltp_weight: LTP weight
        ltd_weight: LTD weight
        time_constant: Time constant
        
    Returns:
        STDP instance
    """
    return STDP(learning_rate, ltp_weight, ltd_weight, time_constant)

def create_structural_plasticity(brain: Optional[BrainCore] = None) -> StructuralPlasticity:
    """Create structural plasticity
    
    Args:
        brain: Brain instance
        
    Returns:
        StructuralPlasticity instance
    """
    return StructuralPlasticity(brain)

class PlasticityUtils:
    """Utility functions for plasticity operations"""
    
    @staticmethod
    def create_hebbian_rule(learning_rate: float = 0.01, time_window: float = 10.0) -> HebbianRule:
        """Create Hebbian rule
        
        Args:
            learning_rate: Learning rate
            time_window: Time window
            
        Returns:
            HebbianRule instance
        """
        return HebbianRule(learning_rate, time_window)
    
    @staticmethod
    def create_stdp_rule(learning_rate: float = 0.01, ltp_weight: float = 0.01,
                         ltd_weight: float = 0.012, time_constant: float = 20.0) -> STDP:
        """Create STDP rule
        
        Args:
            learning_rate: Learning rate
            ltp_weight: LTP weight
            ltd_weight: LTD weight
            time_constant: Time constant
            
        Returns:
            STDP instance
        """
        return STDP(learning_rate, ltp_weight, ltd_weight, time_constant)
    
    @staticmethod
    def create_structural_plasticity(brain: Optional[BrainCore] = None) -> StructuralPlasticity:
        """Create structural plasticity
        
        Args:
            brain: Brain instance
            
        Returns:
            StructuralPlasticity instance
        """
        return StructuralPlasticity(brain)
    
    @staticmethod
    def calculate_plasticity_stats(rules: List[PlasticityRule]) -> Dict[str, Any]:
        """Calculate plasticity statistics
        
        Args:
            rules: List of plasticity rules
            
        Returns:
            Statistics dictionary
        """
        stats = {
            'total_rules': len(rules),
            'enabled_rules': sum(1 for rule in rules if rule.is_enabled()),
            'disabled_rules': sum(1 for rule in rules if not rule.is_enabled()),
            'average_learning_rate': sum(rule.get_learning_rate() for rule in rules) / max(len(rules), 1),
            'rule_names': [rule.get_name() for rule in rules]
        }
        
        return stats
    
    @staticmethod
    def enable_all_rules(rules: List[PlasticityRule]):
        """Enable all rules
        
        Args:
            rules: List of rules
        """
        for rule in rules:
            rule.set_enabled(True)
    
    @staticmethod
    def disable_all_rules(rules: List[PlasticityRule]):
        """Disable all rules
        
        Args:
            rules: List of rules
        """
        for rule in rules:
            rule.set_enabled(False)
    
    @staticmethod
    def set_learning_rate_for_all_rules(rules: List[PlasticityRule], rate: float):
        """Set learning rate for all rules
        
        Args:
            rules: List of rules
            rate: Learning rate
        """
        for rule in rules:
            rule.set_learning_rate(rate)
