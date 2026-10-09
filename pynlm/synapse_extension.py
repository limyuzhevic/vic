"""
Example Synapse Extension Point Plugin

This plugin demonstrates how to extend NLM with custom synapse types using the extension point system.
"""

import sys
from typing import Dict, Any, List
from .plugin_system import SynapseExtensionPoint, NLMPlugin, PluginInfo

class ExampleSynapseExtension(SynapseExtensionPoint):
    """Example extension point for custom synapse types"""
    
    def __init__(self):
        self.synapse_types = {}
        self.enabled = False
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize the extension point with NLM core"""
        try:
            # Store NLM core reference for synapse creation
            self.nlm_core = nlm_core
            
            # Define custom synapse types
            self.synapse_types = {
                'plastic': self._create_plastic_synapse,
                'inhibitory': self._create_inhibitory_synapse,
                'gated': self._create_gated_synapse,
                'adaptive': self._create_adaptive_synapse,
            }
            
            self.enabled = True
            print("Example Synapse Extension initialized successfully")
            return True
        except Exception as e:
            print(f"Failed to initialize Example Synapse Extension: {e}")
            return False
    
    def shutdown(self) -> None:
        """Shutdown the extension point"""
        self.synapse_types.clear()
        self.enabled = False
        print("Example Synapse Extension shut down")
    
    def is_enabled(self) -> bool:
        """Check if the extension point is enabled"""
        return self.enabled
    
    def create_synapse(self, synapse_type: str, parameters: Dict[str, Any]) -> Any:
        """Create a custom synapse"""
        if not self.enabled:
            raise RuntimeError("Synapse extension point is not enabled")
        
        if synapse_type not in self.synapse_types:
            raise ValueError(f"Unknown synapse type: {synapse_type}")
        
        return self.synapse_types[synapse_type](parameters)
    
    def synapse_factory(self, synapse_type: str, **kwargs) -> Any:
        """Factory method for synapse creation"""
        return self.create_synapse(synapse_type, kwargs)
    
    def _create_plastic_synapse(self, parameters: Dict[str, Any]) -> Any:
        """Create a plastic synapse with activity-dependent plasticity"""
        class PlasticSynapse:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'plastic'
                self.base_weight = params.get('base_weight', 1.0)
                self.delay = params.get('delay', 1)
                self.transmission_efficiency = params.get('transmission_efficiency', 0.8)
                self.plasticity_rate = params.get('plasticity_rate', 0.01)
                self.max_weight = params.get('max_weight', 5.0)
                self.min_weight = params.get('min_weight', 0.1)
                self.LTD_threshold = params.get('LTD_threshold', 0.5)
                self.LTP_threshold = params.get('LTP_threshold', 1.5)
                
                # State variables
                self.weight = self.base_weight
                self.time_constant = params.get('time_constant', 0.01)
                self.eligibility_trace = 0.0
                self.post_synaptic_activity = []
                self.pre_synaptic_activity = []
                self.spike_pairs = []  # For STDP
                self.refractory_remaining = 0
                
                # Plasticity parameters
                self.stdp_learning_rate = params.get('stdp_learning_rate', 0.01)
                self.hebbian_strength = params.get('hebbian_strength', 0.1)
                self.moving_average_window = params.get('moving_average_window', 10)
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the plastic synapse"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Decay eligibility trace
                self.eligibility_trace *= 0.9
                
                # Decay moving average activities
                if self.post_synaptic_activity:
                    self.post_synaptic_activity.pop()
                if self.pre_synaptic_activity:
                    self.pre_synaptic_activity.pop()
                
                return False
            
            def record_pre_synaptic_spike(self, time: float) -> None:
                """Record pre-synaptic spike"""
                self.pre_synaptic_activity.append(time)
                self.spike_pairs.append((time, 'pre'))
                
                # Update eligibility trace for STDP
                self.eligibility_trace += self.stdp_learning_rate
            
            def record_post_synaptic_spike(self, time: float) -> None:
                """Record post-synaptic spike"""
                self.post_synaptic_activity.append(time)
                self.spike_pairs.append((time, 'post'))
                
                # Apply STDP for spike pairs
                self._apply_stdp(time)
            
            def _apply_stdp(self, current_time: float) -> None:
                """Apply spike-timing dependent plasticity"""
                for spike_time, spike_type in reversed(self.spike_pairs):
                    if spike_type == 'pre':
                        # Pre-synaptic spike before post-synaptic spike (LTP)
                        time_diff = current_time - spike_time
                        if 0 < time_diff < 50:  # Within 50ms
                            weight_change = self.stdp_learning_rate * (1.0 - time_diff / 50.0)
                            self.weight += weight_change
                            break
                    elif spike_type == 'post':
                        # Post-synaptic spike before pre-synaptic spike (LTD)
                        time_diff = spike_time - current_time
                        if 0 < time_diff < 50:  # Within 50ms
                            weight_change = -self.stdp_learning_rate * (1.0 - time_diff / 50.0)
                            self.weight += weight_change
                            break
                
                # Keep spike pairs manageable
                if len(self.spike_pairs) > 100:
                    self.spike_pairs.pop(0)
                
                # Apply Hebbian learning
                if self.hebbian_strength > 0:
                    self.weight += self.hebbian_strength * len(self.pre_synaptic_activity) * len(self.post_synaptic_activity)
                
                # Apply weight bounds
                self.weight = max(self.min_weight, min(self.weight, self.max_weight))
            
            def inject_current(self, current: float) -> None:
                """Inject current into postsynaptic neuron"""
                # This would be called by the NLM core
                pass
            
            def get_weight(self) -> float:
                """Get current synaptic weight"""
                return self.weight
            
            def set_weight(self, weight: float) -> None:
                """Set synaptic weight"""
                self.weight = max(self.min_weight, min(weight, self.max_weight))
            
            def get_state(self) -> Dict[str, Any]:
                """Get synapse state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'weight': self.weight,
                    'base_weight': self.base_weight,
                    'delay': self.delay,
                    'transmission_efficiency': self.transmission_efficiency,
                    'plasticity_rate': self.plasticity_rate,
                    'eligibility_trace': self.eligibility_trace,
                    'pre_synaptic_activity': self.pre_synaptic_activity,
                    'post_synaptic_activity': self.post_synaptic_activity,
                    'spike_pairs': self.spike_pairs,
                    'refractory_remaining': self.refractory_remaining
                }
            
            def __repr__(self):
                return f"PlasticSynapse(id={self.id}, type={self.type}, weight={self.weight:.3f})"
        
        return PlasticSynapse(parameters)
    
    def _create_inhibitory_synapse(self, parameters: Dict[str, Any]) -> Any:
        """Create an inhibitory synapse with shunting inhibition"""
        class InhibitorySynapse:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'inhibitory'
                self.base_weight = params.get('base_weight', -1.5)
                self.delay = params.get('delay', 2)
                self.inhibition_strength = params.get('inhibition_strength', 0.8)
                self.decay_time = params.get('decay_time', 0.02)
                self.threshold_reduction = params.get('threshold_reduction', 0.5)
                
                # State variables
                self.weight = self.base_weight
                self.inhibition_pulse = 0.0
                self.inhibition_decay = 0.0
                self.refractory_remaining = 0
                self.inhibition_history = []
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the inhibitory synapse"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Decay inhibition
                self.inhibition_decay *= 0.9
                self.inhibition_pulse *= 0.9
                
                return False
            
            def record_post_synaptic_spike(self, time: float) -> None:
                """Record post-synaptic spike to generate inhibition"""
                self.inhibition_pulse = self.inhibition_strength
                self.inhibition_history.append(time)
                
                # Generate shunting inhibition pulse
                self.refractory_remaining = int(self.delay)
            
            def get_inhibition(self) -> float:
                """Get current inhibition strength"""
                return self.inhibition_pulse + self.inhibition_decay
            
            def inject_current(self, current: float) -> None:
                """Inject current into postsynaptic neuron (inhibitory)"""
                pass
            
            def get_state(self) -> Dict[str, Any]:
                """Get synapse state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'weight': self.weight,
                    'base_weight': self.base_weight,
                    'delay': self.delay,
                    'inhibition_strength': self.inhibition_strength,
                    'decay_time': self.decay_time,
                    'inhibition_pulse': self.inhibition_pulse,
                    'inhibition_decay': self.inhibition_decay,
                    'inhibition_history': self.inhibition_history
                }
        
        return InhibitorySynapse(parameters)
    
    def _create_gated_synapse(self, parameters: Dict[str, Any]) -> Any:
        """Create a gated synapse with conditional transmission"""
        class GatedSynapse:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'gated'
                self.base_weight = params.get('base_weight', 1.0)
                self.delay = params.get('delay', 1)
                self.gate_threshold = params.get('gate_threshold', 0.5)
                self.gate_sensitivity = params.get('gate_sensitivity', 2.0)
                self.gate_decay = params.get('gate_decay', 0.95)
                
                # State variables
                self.weight = self.base_weight
                self.gate_state = 0.0
                self.gate_history = []
                self.refractory_remaining = 0
                self.transmission_enabled = True
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the gated synapse"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Decay gate state
                self.gate_state *= self.gate_decay
                
                return False
            
            def update_gate(self, input_value: float) -> None:
                """Update gate state based on input"""
                if input_value > self.gate_threshold:
                    self.gate_state = min(1.0, self.gate_state + self.gate_sensitivity * (input_value - self.gate_threshold))
                    self.transmission_enabled = True
                else:
                    self.gate_state *= 0.5
                    self.transmission_enabled = False
                
                self.gate_history.append(self.gate_state)
            
            def can_transmit(self) -> bool:
                """Check if synapse can transmit signal"""
                return self.transmission_enabled and self.gate_state > 0.1
            
            def inject_current(self, current: float) -> None:
                """Inject current into postsynaptic neuron (gated)"""
                if self.can_transmit():
                    # Apply gated transmission
                    effective_current = current * self.gate_state * self.weight
                    # This would be handled by the NLM core
            
            def get_state(self) -> Dict[str, Any]:
                """Get synapse state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'weight': self.weight,
                    'base_weight': self.base_weight,
                    'delay': self.delay,
                    'gate_threshold': self.gate_threshold,
                    'gate_sensitivity': self.gate_sensitivity,
                    'gate_decay': self.gate_decay,
                    'gate_state': self.gate_state,
                    'gate_history': self.gate_history,
                    'transmission_enabled': self.transmission_enabled
                }
        
        return GatedSynapse(parameters)
    
    def _create_adaptive_synapse(self, parameters: Dict[str, Any]) -> Any:
        """Create an adaptive synapse with dynamic properties"""
        class AdaptiveSynapse:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'adaptive'
                self.base_weight = params.get('base_weight', 1.0)
                self.delay = params.get('delay', 1)
                self.adaptation_rate = params.get('adaptation_rate', 0.01)
                self.history_window = params.get('history_window', 20)
                self.prediction_strength = params.get('prediction_strength', 0.5)
                
                # State variables
                self.weight = self.base_weight
                self.predicted_weight = self.base_weight
                self.actual_weight = self.base_weight
                self.error_history = []
                self.prediction_history = []
                self.refractory_remaining = 0
                self.adaptation_accumulator = 0.0
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the adaptive synapse"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Update predicted weight based on recent activity
                self._update_prediction()
                
                # Apply adaptation
                self._apply_adaptation()
                
                return False
            
            def _update_prediction(self) -> None:
                """Update weight prediction based on recent activity"""
                if self.prediction_history:
                    # Simple moving average prediction
                    avg_pred = sum(self.prediction_history[-self.history_window:]) / len(self.prediction_history[-self.history_window:])
                    self.predicted_weight = avg_pred
                else:
                    self.predicted_weight = self.base_weight
            
            def _apply_adaptation(self) -> None:
                """Apply adaptive learning based on prediction error"""
                if self.error_history:
                    # Calculate prediction error
                    error = self.actual_weight - self.predicted_weight
                    self.error_history.append(error)
                    
                    # Keep error history manageable
                    if len(self.error_history) > 100:
                        self.error_history.pop(0)
                    
                    # Update weight based on error
                    if abs(error) > 0.1:
                        adaptation = self.adaptation_rate * error * self.prediction_strength
                        self.weight += adaptation
                        self.adaptation_accumulator += abs(adaptation)
                
                # Bound weight
                self.weight = max(0.1, min(self.weight, 5.0))
            
            def record_spike_pair(self, pre_time: float, post_time: float, weight_change: float) -> None:
                """Record spike pair for learning"""
                self.prediction_history.append(weight_change)
                self.actual_weight = self.weight
                
                # Keep history manageable
                if len(self.prediction_history) > 100:
                    self.prediction_history.pop(0)
                if len(self.error_history) > 100:
                    self.error_history.pop(0)
            
            def inject_current(self, current: float) -> None:
                """Inject current into postsynaptic neuron (adaptive)"""
                pass
            
            def get_state(self) -> Dict[str, Any]:
                """Get synapse state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'weight': self.weight,
                    'base_weight': self.base_weight,
                    'delay': self.delay,
                    'predicted_weight': self.predicted_weight,
                    'adaptation_rate': self.adaptation_rate,
                    'history_window': self.history_window,
                    'prediction_strength': self.prediction_strength,
                    'error_history': self.error_history,
                    'prediction_history': self.prediction_history,
                    'adaptation_accumulator': self.adaptation_accumulator
                }
        
        return AdaptiveSynapse(parameters)

# Main plugin class
class ExampleSynapsePlugin(NLMPlugin):
    """Plugin for example synapse extension points"""
    
    def __init__(self):
        super().__init__()
        self.info = PluginInfo(
            name="Example Synapse Plugin",
            version="1.0.0",
            description="Plugin providing custom synapse extension points for NLM",
            author="NLM Team",
            capabilities=["synapse"],
            dependencies=[],
            entry_point="main"
        )
        self.synapse_extension = None
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize plugin with NLM core"""
        if not super().initialize(nlm_core):
            return False
        
        # Create synapse extension point
        self.synapse_extension = ExampleSynapseExtension()
        
        # Initialize the extension point
        if self.synapse_extension.initialize(nlm_core):
            print("Example Synapse Plugin initialized successfully")
            return True
        else:
            print("Failed to initialize Example Synapse Extension")
            return False
    
    def shutdown(self) -> None:
        """Shutdown plugin"""
        if self.synapse_extension:
            self.synapse_extension.shutdown()
            self.synapse_extension = None
        super().shutdown()
        print("Example Synapse Plugin shut down")
    
    def get_synapse_extension(self) -> Optional[ExampleSynapseExtension]:
        """Get the synapse extension point"""
        return self.synapse_extension

# Export plugin class
__all__ = ['ExampleSynapsePlugin', 'ExampleSynapseExtension']
