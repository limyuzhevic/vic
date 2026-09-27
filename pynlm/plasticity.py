from dataclasses import dataclass, field
"""
Plasticity Rules - STDP and Hebbian Learning
=============================================

Implementation of spike-timing-dependent plasticity (STDP) and
Hebbian learning rules with neuromodulatory modulation and
structural plasticity.
"""

from typing import List, Dict, Any, Optional
import math
@dataclass
class SynapticState:
    """Synaptic state for plasticity computations."""
    
    weight: float = 1.0           # Synaptic weight
    pre_trace: float = 0.0       # Pre-synaptic spike trace
    post_trace: float = 0.0      # Post-synaptic spike trace
    last_pre_spike: float = -1.0 # Last pre-synaptic spike time
    last_post_spike: float = -1.0 # Last post-synaptic spike time
    history_pre: List[float] = field(default_factory=list)  # Pre-synaptic spike history
    history_post: List[float] = field(default_factory=list) # Post-synaptic spike history
    
    def update(self, dt: float):
        """Update synaptic traces."""
        self.pre_trace *= math.exp(-dt / 20.0)  # Tau = 20ms
        self.post_trace *= math.exp(-dt / 20.0)
        
    def record_pre_spike(self, time: float):
        """Record pre-synaptic spike."""
        self.last_pre_spike = time
        self.history_pre.append(time)
        if len(self.history_pre) > 100:
            self.history_pre.pop(0)
            
    def record_post_spike(self, time: float):
        """Record post-synaptic spike."""
        self.last_post_spike = time
        self.history_post.append(time)
        if len(self.history_post) > 100:
            self.history_post.pop(0)
class STDP:
    """
    Spike-Timing-Dependent Plasticity (STDP) rule.
    
    Implements the classic STDP rule where synaptic weight changes
    depend on the relative timing of pre- and post-synaptic spikes:
    - Pre before post: LTP (long-term potentiation)
    - Post before pre: LTD (long-term depression)
    
    Attributes:
        ltp_weight (float): LTP weight change
        ltd_weight (float): LTD weight change
        tau (float): Time constant for traces
    """
    
    def __init__(self, ltp_weight: float = 0.01, ltd_weight: float = 0.012, 
                 tau: float = 20.0):
        """
        Initialize STDP rule.
        
        Args:
            ltp_weight (float): LTP weight change per spike pair
            ltd_weight (float): LTD weight change per spike pair
            tau (float): Time constant for traces (ms)
        """
        self.ltp_weight = ltp_weight
        self.ltd_weight = ltd_weight
        self.tau = tau
        
    def configure(self, ltp_weight: float, ltd_weight: float, tau: float):
        """
        Configure STDP parameters.
        
        Args:
            ltp_weight (float): LTP weight change
            ltd_weight (float): LTD weight change
            tau (float): Time constant
        """
        self.ltp_weight = ltp_weight
        self.ltd_weight = ltd_weight
        self.tau = tau
        
    def update(self, synapse: Any, pre_spike_history: List[float],
               post_spike_history: List[float], dt: float):
        """
        Update synaptic weight based on STDP.
        
        Args:
            synapse (Any): Synapse object to update
            pre_spike_history (List[float]): History of pre-synaptic spikes
            post_spike_history (List[float]): History of post-synaptic spikes
            dt (float): Time step duration
        """
        # Get current synaptic state
        if hasattr(synapse, 'get_state'):
            state = synapse.get_state()
        else:
            # Create or update synaptic state
            if not hasattr(synapse, 'state'):
                synapse.state = SynapticState()
            state = synapse.state
            
        # Update traces
        state.update(dt)
        
        # Calculate STDP weight change
        weight_change = 0.0
        
        # LTP: pre before post
        if pre_spike_history and post_spike_history:
            for pre_time in pre_spike_history:
                for post_time in post_spike_history:
                    if post_time > pre_time:
                        # LTP contribution
                        delta_t = (post_time - pre_time) / 1000.0  # Convert to seconds
                        ltp = self.ltp_weight * math.exp(-delta_t / self.tau)
                        weight_change += ltp
                        
        # LTD: post before pre
        if post_spike_history and pre_spike_history:
            for post_time in post_spike_history:
                for pre_time in pre_spike_history:
                    if pre_time > post_time:
                        # LTD contribution
                        delta_t = (pre_time - post_time) / 1000.0  # Convert to seconds
                        ltd = self.ltd_weight * math.exp(-delta_t / self.tau)
                        weight_change -= ltd
                        
        # Apply weight change with bounds
        new_weight = state.weight + weight_change
        new_weight = max(0.0, min(1.0, new_weight))  # Clamp to [0, 1]
        
        # Update synapse weight
        if hasattr(synapse, 'set_weight'):
            synapse.set_weight(new_weight)
        elif hasattr(synapse, 'weight'):
            synapse.weight = new_weight
            
    def set_weight(self, synapse: Any, weight: float):
        """
        Set synaptic weight.
        
        Args:
            synapse (Any): Synapse object
            weight (float): New weight value
        """
        if hasattr(synapse, 'set_weight'):
            synapse.set_weight(weight)
        elif hasattr(synapse, 'weight'):
            synapse.weight = weight
            
    def get_ltp_weight(self) -> float:
        """Get LTP weight."""
        return self.ltp_weight
        
    def get_ltd_weight(self) -> float:
        """Get LTD weight."""
        return self.ltd_weight
        
    def get_tau(self) -> float:
        """Get time constant."""
        return self.tau
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get STDP state.
        
        Returns:
            Dict[str, Any]: STDP state
        """
        return {
            "ltp_weight": self.ltp_weight,
            "ltd_weight": self.ltd_weight,
            "tau": self.tau
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set STDP state.
        
        Args:
            state (Dict[str, Any]): STDP state
        """
        self.ltp_weight = state.get("ltp_weight", self.ltp_weight)
        self.ltd_weight = state.get("ltd_weight", self.ltd_weight)
        self.tau = state.get("tau", self.tau)
class Hebbian:
    """
    Hebbian learning rule.
    
    Implements the classic Hebbian learning rule: "Neurons that
    fire together, wire together." Weight increases when pre- and
    post-synaptic neurons fire simultaneously or nearly simultaneously.
    
    Attributes:
        learning_rate (float): Learning rate
        trace_decay (float): Trace decay rate
    """
    
    def __init__(self, learning_rate: float = 0.01, trace_decay: float = 0.01):
        """
        Initialize Hebbian learning rule.
        
        Args:
            learning_rate (float): Learning rate
            trace_decay (float): Trace decay rate
        """
        self.learning_rate = learning_rate
        self.trace_decay = trace_decay
        
    def update(self, synapse: Any, pre_spike_history: List[float],
               post_spike_history: List[float], dt: float):
        """
        Update synaptic weight based on Hebbian rule.
        
        Args:
            synapse (Any): Synapse object to update
            pre_spike_history (List[float]): History of pre-synaptic spikes
            post_spike_history (List[float]): History of post-synaptic spikes
            dt (float): Time step duration
        """
        # Get current synaptic state
        if hasattr(synapse, 'get_state'):
            state = synapse.get_state()
        else:
            if not hasattr(synapse, 'state'):
                synapse.state = SynapticState()
            state = synapse.state
            
        # Calculate correlation between pre and post spikes
        correlation = 0.0
        
        # Simple correlation: count simultaneous spikes
        time_window = 10.0  # 10ms time window
        
        for pre_time in pre_spike_history:
            for post_time in post_spike_history:
                if abs(pre_time - post_time) <= time_window:
                    correlation += 1.0
                    
        # Apply Hebbian learning
        weight_change = self.learning_rate * correlation
        
        # Apply weight change with bounds
        new_weight = state.weight + weight_change
        new_weight = max(0.0, min(1.0, new_weight))  # Clamp to [0, 1]
        
        # Update synapse weight
        if hasattr(synapse, 'set_weight'):
            synapse.set_weight(new_weight)
        elif hasattr(synapse, 'weight'):
            synapse.weight = new_weight
            
    def get_state(self) -> Dict[str, Any]:
        """
        Get Hebbian state.
        
        Returns:
            Dict[str, Any]: Hebbian state
        """
        return {
            "learning_rate": self.learning_rate,
            "trace_decay": self.trace_decay
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set Hebbian state.
        
        Args:
            state (Dict[str, Any]): Hebbian state
        """
        self.learning_rate = state.get("learning_rate", self.learning_rate)
        self.trace_decay = state.get("trace_decay", self.trace_decay)
class StructuralPlasticity:
    """
    Structural plasticity for synaptogenesis and pruning.
    
    Implements structural changes to neural connectivity:
    - Synaptogenesis (formation of new synapses)
    - Pruning (elimination of synapses)
    - Dendritic spine dynamics
    - Activity-dependent structural changes
    
    Attributes:
        synaptogenesis_rate (float): Rate of new synapse formation
        pruning_rate (float): Rate of synapse elimination
        min_weight (float): Minimum synaptic weight
        max_weight (float): Maximum synaptic weight
    """
    
    def __init__(self, synaptogenesis_rate: float = 0.0001,
                 pruning_rate: float = 0.00001, min_weight: float = 0.1,
                 max_weight: float = 0.9):
        """
        Initialize structural plasticity.
        
        Args:
            synaptogenesis_rate (float): Rate of new synapse formation
            pruning_rate (float): Rate of synapse elimination
            min_weight (float): Minimum synaptic weight
            max_weight (float): Maximum synaptic weight
        """
        self.synaptogenesis_rate = synaptogenesis_rate
        self.pruning_rate = pruning_rate
        self.min_weight = min_weight
        self.max_weight = max_weight
        
    def update(self, brain: Any, rng: Any, dt: float):
        """
        Update structural plasticity.
        
        Args:
            brain (Any): Brain object for access to regions and neurons
            rng (Any): Random number generator
            dt (float): Time step duration
        """
        # This is a simplified implementation
        # In a full implementation, this would:
        # 1. Add new synapses based on activity
        # 2. Prune weak synapses
        # 3. Update dendritic structure
        # 4. Modify synaptic strength
        
        pass
        
    def set_synaptogenesis_rate(self, rate: float):
        """Set synaptogenesis rate."""
        self.synaptogenesis_rate = rate
        
    def set_pruning_rate(self, rate: float):
        """Set pruning rate."""
        self.pruning_rate = rate
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get structural plasticity state.
        
        Returns:
            Dict[str, Any]: Structural plasticity state
        """
        return {
            "synaptogenesis_rate": self.synaptogenesis_rate,
            "pruning_rate": self.pruning_rate,
            "min_weight": self.min_weight,
            "max_weight": self.max_weight
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set structural plasticity state.
        
        Args:
            state (Dict[str, Any]): Structural plasticity state
        """
        self.synaptogenesis_rate = state.get("synaptogenesis_rate", 
                                             self.synaptogenesis_rate)
        self.pruning_rate = state.get("pruning_rate", self.pruning_rate)
        self.min_weight = state.get("min_weight", self.min_weight)
        self.max_weight = state.get("max_weight", self.max_weight)
class PlasticityRule:
    """
    Complete plasticity system integrating all plasticity rules.
    
    Coordinates STDP, Hebbian, and structural plasticity with
    neuromodulatory modulation and activity-dependent regulation.
    
    Attributes:
        stdp (STDP): STDP rule
        hebbian (Hebbian): Hebbian learning rule
        structural (StructuralPlasticity): Structural plasticity
        learning_rate (float): Global learning rate
    """
    
    def __init__(self, learning_rate: float = 0.01):
        """
        Initialize plasticity system.
        
        Args:
            learning_rate (float): Global learning rate
        """
        self.stdp = STDP(ltp_weight=0.01, ltd_weight=0.012)
        self.hebbian = Hebbian(learning_rate=learning_rate)
        self.structural = StructuralPlasticity()
        
        self.global_modulation = 1.0
        
    def initialize(self):
        """Initialize plasticity system."""
        pass
        
    def update(self, dt: float):
        """
        Update plasticity system.
        
        Args:
            dt (float): Time step duration
        """
        # Apply global modulation
        self.stdp.configure(
            self.stdp.ltp_weight * self.global_modulation,
            self.stdp.ltd_weight * self.global_modulation,
            self.stdp.tau
        )
        
        # Update plasticity rules based on neural activity
        # This would normally be triggered by spike events
        
    def record_spike_pair(self, synapse: Any, pre_spike: float, post_spike: float):
        """
        Record spike pair for plasticity computation.
        
        Args:
            synapse (Any): Synapse object
            pre_spike (float): Pre-synaptic spike time
            post_spike (float): Post-synaptic spike time
        """
        if hasattr(synapse, 'state'):
            if pre_spike > post_spike:
                # Pre before post: potential LTP
                synapse.state.record_pre_spike(pre_spike)
                synapse.state.record_post_spike(post_spike)
            else:
                # Post before pre: potential LTD
                synapse.state.record_pre_spike(pre_spike)
                synapse.state.record_post_spike(post_spike)
                
    def get_stdp(self) -> STDP:
        """Get STDP rule."""
        return self.stdp
        
    def get_hebbian(self) -> Hebbian:
        """Get Hebbian rule."""
        return self.hebbian
        
    def get_structural(self) -> StructuralPlasticity:
        """Get structural plasticity."""
        return self.structural
        
    def reset(self):
        """Reset plasticity system."""
        pass
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get plasticity system state.
        
        Returns:
            Dict[str, Any]: Plasticity system state
        """
        return {
            "stdp": self.stdp.get_state(),
            "hebbian": self.hebbian.get_state(),
            "structural": self.structural.get_state(),
            "learning_rate": self.hebbian.learning_rate,
            "global_modulation": self.global_modulation
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set plasticity system state.
        
        Args:
            state (Dict[str, Any]): Plasticity system state
        """
        self.stdp.set_state(state["stdp"])
        self.hebbian.set_state(state["hebbian"])
        self.structural.set_state(state["structural"])
        self.global_modulation = state.get("global_modulation", 1.0)
        
    def __repr__(self) -> str:
        return f"<PlasticityRule stdp={self.stdp.ltp_weight:.3f} hebbian={self.hebbian.learning_rate:.3f}>"
