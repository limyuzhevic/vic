"""
Example Plasticity Extension Point Plugin

This plugin demonstrates how to extend NLM with custom plasticity rules using the extension point system.
"""

import sys
from typing import Dict, Any, List
from .plugin_system import PlasticityExtensionPoint, NLMPlugin, PluginInfo

class ExamplePlasticityExtension(PlasticityExtensionPoint):
    """Example extension point for custom plasticity rules"""
    
    def __init__(self):
        self.plasticity_rules = {}
        self.enabled = False
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize the extension point with NLM core"""
        try:
            # Store NLM core reference for plasticity calculation
            self.nlm_core = nlm_core
            
            # Define custom plasticity rules
            self.plasticity_rules = {
                'stdp': self._create_stdp_rule,
                'hebbian': self._create_hebbian_rule,
                'pairing_based': self._create_pairing_based_rule,
                'homeostatic': self._create_homeostatic_rule,
            }
            
            self.enabled = True
            print("Example Plasticity Extension initialized successfully")
            return True
        except Exception as e:
            print(f"Failed to initialize Example Plasticity Extension: {e}")
            return False
    
    def shutdown(self) -> None:
        """Shutdown the extension point"""
        self.plasticity_rules.clear()
        self.enabled = False
        print("Example Plasticity Extension shut down")
    
    def is_enabled(self) -> bool:
        """Check if the extension point is enabled"""
        return self.enabled
    
    def apply_plasticity(self, synapse: Any, pre_spikes: List[float], post_spikes: List[float], 
                        timestep: float) -> float:
        """Apply custom plasticity rule"""
        if not self.enabled:
            raise RuntimeError("Plasticity extension point is not enabled")
        
        # Determine which plasticity rule to use based on synapse type
        synapse_type = getattr(synapse, 'type', 'stdp')
        
        if synapse_type in self.plasticity_rules:
            return self.plasticity_rules[synapse_type](synapse, pre_spikes, post_spikes, timestep)
        else:
            return self.plasticity_rules['stdp'](synapse, pre_spikes, post_spikes, timestep)
    
    def plasticity_factory(self, rule_type: str, **kwargs) -> Any:
        """Factory method for plasticity rule creation"""
        return self.apply_plasticity(None, [], [], 0.001)  # This is a simplified example
    
    def _create_stdp_rule(self, synapse: Any, pre_spikes: List[float], post_spikes: List[float], 
                         timestep: float) -> float:
        """Create a spike-timing dependent plasticity rule"""
        class STDPRule:
            def __init__(self):
                self.LTP_rate = 0.01
                self.LTD_rate = 0.012
                self.time_constant = 20.0
                self.weight = getattr(synapse, 'weight', 1.0)
            
            def apply(self, pre_spikes: List[float], post_spikes: List[float], timestep: float) -> float:
                """Apply STDP rule"""
                weight_change = 0.0
                
                # For each post-synaptic spike, check for preceding pre-synaptic spikes
                for post_time in post_spikes:
                    for pre_time in reversed(pre_spikes):
                        time_diff = post_time - pre_time
                        
                        if 0 < time_diff < self.time_constant:
                            # Long-term potentiation (LTP): pre before post
                            weight_change += self.LTP_rate * (1.0 - time_diff / self.time_constant)
                        elif -self.time_constant < time_diff < 0:
                            # Long-term depression (LTD): post before pre
                            weight_change -= self.LTD_rate * (1.0 - abs(time_diff) / self.time_constant)
                        
                        break  # Only consider most recent pre-synaptic spike
                
                return weight_change
        
        rule = STDPRule()
        return rule.apply(pre_spikes, post_spikes, timestep)
    
    def _create_hebbian_rule(self, synapse: Any, pre_spikes: List[float], post_spikes: List[float], 
                           timestep: float) -> float:
        """Create a Hebbian plasticity rule"""
        class HebbianRule:
            def __init__(self):
                self.strength = 0.1
                self.threshold = 0.5
                self.decay_rate = 0.99
                self.weight = getattr(synapse, 'weight', 1.0)
            
            def apply(self, pre_spikes: List[float], post_spikes: List[float], timestep: float) -> float:
                """Apply Hebbian rule"""
                weight_change = 0.0
                
                # Hebbian learning: if both pre and post are active, strengthen connection
                if pre_spikes and post_spikes:
                    # Measure co-activity
                    co_activity = len(pre_spikes) * len(post_spikes)
                    
                    # Apply Hebbian learning
                    if co_activity > self.threshold:
                        weight_change = self.strength * co_activity
                
                # Decay over time
                self.strength *= self.decay_rate
                
                return weight_change
        
        rule = HebbianRule()
        return rule.apply(pre_spikes, post_spikes, timestep)
    
    def _create_pairing_based_rule(self, synapse: Any, pre_spikes: List[float], post_spikes: List[float], 
                                  timestep: float) -> float:
        """Create a pairing-based plasticity rule"""
        class PairingRule:
            def __init__(self):
                self.pairing_strength = 0.05
                self.time_window = 50.0
                self.weight = getattr(synapse, 'weight', 1.0)
                self.paired_spikes = []
            
            def apply(self, pre_spikes: List[float], post_spikes: List[float], timestep: float) -> float:
                """Apply pairing-based rule"""
                weight_change = 0.0
                
                # Find spike pairs within time window
                for pre_time in pre_spikes:
                    for post_time in post_spikes:
                        time_diff = abs(post_time - pre_time)
                        
                        if time_diff < self.time_window:
                            # Create a paired spike record
                            pair_id = f"{pre_time:.2f}_{post_time:.2f}"
                            if pair_id not in self.paired_spikes:
                                self.paired_spikes.append(pair_id)
                                
                                # Apply pairing-based learning
                                if post_time > pre_time:  # Pre before post
                                    weight_change += self.pairing_strength * (1.0 - time_diff / self.time_window)
                                else:  # Post before pre
                                    weight_change -= self.pairing_strength * (1.0 - time_diff / self.time_window)
                
                # Keep paired spikes manageable
                if len(self.paired_spikes) > 100:
                    self.paired_spikes.pop(0)
                
                return weight_change
        
        rule = PairingRule()
        return rule.apply(pre_spikes, post_spikes, timestep)
    
    def _create_homeostatic_rule(self, synapse: Any, pre_spikes: List[float], post_spikes: List[float], 
                                timestep: float) -> float:
        """Create a homeostatic plasticity rule"""
        class HomeostaticRule:
            def __init__(self):
                self.target_weight = 1.0
                self.learning_rate = 0.01
                self.stability_threshold = 0.1
                self.weight = getattr(synapse, 'weight', 1.0)
                self.stability_counter = 0
            
            def apply(self, pre_spikes: List[float], post_spikes: List[float], timestep: float) -> float:
                """Apply homeostatic rule"""
                weight_change = 0.0
                
                # Calculate deviation from target weight
                deviation = self.weight - self.target_weight
                
                if abs(deviation) > self.stability_threshold:
                    # Apply homeostatic adjustment
                    weight_change = -self.learning_rate * deviation
                    
                    # Increase stability counter
                    self.stability_counter += 1
                    
                    # Reduce learning rate if stable for long time
                    if self.stability_counter > 100:
                        self.learning_rate *= 0.9
                else:
                    # Reset stability counter
                    self.stability_counter = 0
                
                # Ensure weight stays within bounds
                self.weight = max(0.1, min(self.weight, 5.0))
                
                return weight_change
        
        rule = HomeostaticRule()
        return rule.apply(pre_spikes, post_spikes, timestep)

# Main plugin class
class ExamplePlasticityPlugin(NLMPlugin):
    """Plugin for example plasticity extension points"""
    
    def __init__(self):
        super().__init__()
        self.info = PluginInfo(
            name="Example Plasticity Plugin",
            version="1.0.0",
            description="Plugin providing custom plasticity extension points for NLM",
            author="NLM Team",
            capabilities=["plasticity"],
            dependencies=[],
            entry_point="main"
        )
        self.plasticity_extension = None
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize plugin with NLM core"""
        if not super().initialize(nlm_core):
            return False
        
        # Create plasticity extension point
        self.plasticity_extension = ExamplePlasticityExtension()
        
        # Initialize the extension point
        if self.plasticity_extension.initialize(nlm_core):
            print("Example Plasticity Plugin initialized successfully")
            return True
        else:
            print("Failed to initialize Example Plasticity Extension")
            return False
    
    def shutdown(self) -> None:
        """Shutdown plugin"""
        if self.plasticity_extension:
            self.plasticity_extension.shutdown()
            self.plasticity_extension = None
        super().shutdown()
        print("Example Plasticity Plugin shut down")
    
    def get_plasticity_extension(self) -> Optional[ExamplePlasticityExtension]:
        """Get the plasticity extension point"""
        return self.plasticity_extension

# Export plugin class
__all__ = ['ExamplePlasticityPlugin', 'ExamplePlasticityExtension']
