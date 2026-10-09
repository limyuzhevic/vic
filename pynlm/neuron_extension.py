"""
Example Neuron Extension Point Plugin

This plugin demonstrates how to extend NLM with custom neuron types using the extension point system.
"""

import sys
from typing import Dict, Any
from .plugin_system import NeuronExtensionPoint, NLMPlugin, PluginInfo

class ExampleNeuronExtension(NeuronExtensionPoint):
    """Example extension point for custom neuron types"""
    
    def __init__(self):
        self.neuron_types = {}
        self.enabled = False
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize the extension point with NLM core"""
        try:
            # Store NLM core reference for neuron creation
            self.nlm_core = nlm_core
            
            # Define custom neuron types
            self.neuron_types = {
                'artificial': self._create_artificial_neuron,
                'reservoir': self._create_reservoir_neuron,
                'adaptive': self._create_adaptive_neuron,
            }
            
            self.enabled = True
            print("Example Neuron Extension initialized successfully")
            return True
        except Exception as e:
            print(f"Failed to initialize Example Neuron Extension: {e}")
            return False
    
    def shutdown(self) -> None:
        """Shutdown the extension point"""
        self.neuron_types.clear()
        self.enabled = False
        print("Example Neuron Extension shut down")
    
    def is_enabled(self) -> bool:
        """Check if the extension point is enabled"""
        return self.enabled
    
    def create_neuron(self, neuron_type: str, parameters: Dict[str, Any]) -> Any:
        """Create a custom neuron"""
        if not self.enabled:
            raise RuntimeError("Neuron extension point is not enabled")
        
        if neuron_type not in self.neuron_types:
            raise ValueError(f"Unknown neuron type: {neuron_type}")
        
        return self.neuron_types[neuron_type](parameters)
    
    def neuron_factory(self, neuron_type: str, **kwargs) -> Any:
        """Factory method for neuron creation"""
        return self.create_neuron(neuron_type, kwargs)
    
    def _create_artificial_neuron(self, parameters: Dict[str, Any]) -> Any:
        """Create an artificial neuron with custom dynamics"""
        class ArtificialNeuron:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'artificial'
                self.threshold = params.get('threshold', 1.0)
                self.time_constant = params.get('time_constant', 0.01)
                self.adaptation_rate = params.get('adaptation_rate', 0.1)
                self.firing_threshold = params.get('firing_threshold', 2.0)
                self.refractory_period = params.get('refractory_period', 10)
                
                # Custom dynamics parameters
                self.adaptive_threshold = params.get('adaptive_threshold', False)
                self.input_scaling = params.get('input_scaling', 1.0)
                self.output_scaling = params.get('output_scaling', 1.0)
                
                # State variables
                self.membrane_potential = 0.0
                self.resting_potential = 0.0
                self.refractory_remaining = 0
                self.firing_count = 0
                self.adaptation = 0.0
                
                # Activity tracking
                self.spike_times = []
                self.input_history = []
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the artificial neuron"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Custom LIF dynamics with adaptation
                self.membrane_potential += (self.resting_potential - self.membrane_potential) * dt / self.time_constant
                self.membrane_potential *= self.input_scaling
                
                # Add adaptation
                if self.adaptive_threshold:
                    self.threshold = self.firing_threshold + self.adaptation_rate * self.adaptation
                
                # Check for spike
                if self.membrane_potential >= self.threshold:
                    self.membrane_potential = self.resting_potential
                    self.refractory_remaining = self.refractory_period
                    self.firing_count += 1
                    self.spike_times.append(current_time)
                    self.adaptation += 1.0
                    
                    return True
                
                return False
            
            def inject_current(self, current: float) -> None:
                """Inject current into the neuron"""
                self.membrane_potential += current * self.output_scaling
            
            def get_state(self) -> Dict[str, Any]:
                """Get neuron state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'membrane_potential': self.membrane_potential,
                    'resting_potential': self.resting_potential,
                    'threshold': self.threshold,
                    'firing_count': self.firing_count,
                    'adaptation': self.adaptation,
                    'refractory_remaining': self.refractory_remaining,
                    'spike_times': self.spike_times,
                    'input_history': self.input_history
                }
            
            def __repr__(self):
                return f"ArtificialNeuron(id={self.id}, type={self.type}, v={self.membrane_potential:.3f})"
        
        return ArtificialNeuron(parameters)
    
    def _create_reservoir_neuron(self, parameters: Dict[str, Any]) -> Any:
        """Create a reservoir neuron with high connectivity"""
        class ReservoirNeuron:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'reservoir'
                self.threshold = params.get('threshold', 1.5)
                self.time_constant = params.get('time_constant', 0.02)
                self.input_gain = params.get('input_gain', 2.0)
                self.output_gain = params.get('output_gain', 1.5)
                self.spike_regularization = params.get('spike_regularization', 0.9)
                
                # State variables
                self.membrane_potential = 0.0
                self.resting_potential = 0.0
                self.refractory_remaining = 0
                self.firing_count = 0
                self.spike_history = []
                
                # Reservoir dynamics parameters
                self.recurrent_excitation = params.get('recurrent_excitation', 0.1)
                self.inhibitory_weight = params.get('inhibitory_weight', -0.5)
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the reservoir neuron"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Reservoir dynamics with recurrent connections
                input_current = self.input_gain
                if self.spike_history:
                    # Add recurrent excitation from recent spikes
                    time_since_last_spike = current_time - self.spike_history[-1]
                    if time_since_last_spike < 10.0:  # Within 10ms
                        input_current += self.recurrent_excitation
                
                # LIF dynamics with regularization
                self.membrane_potential += (self.resting_potential - self.membrane_potential) * dt / self.time_constant
                self.membrane_potential += input_current * dt
                
                # Apply spike regularization
                if len(self.spike_history) > 10:
                    self.threshold *= self.spike_regularization
                
                # Check for spike
                if self.membrane_potential >= self.threshold:
                    self.membrane_potential = self.resting_potential
                    self.refractory_remaining = 5
                    self.firing_count += 1
                    self.spike_history.append(current_time)
                    
                    return True
                
                return False
            
            def inject_current(self, current: float) -> None:
                """Inject current into the neuron"""
                self.membrane_potential += current * self.output_gain
            
            def get_state(self) -> Dict[str, Any]:
                """Get neuron state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'membrane_potential': self.membrane_potential,
                    'resting_potential': self.resting_potential,
                    'threshold': self.threshold,
                    'firing_count': self.firing_count,
                    'spike_history': self.spike_history,
                    'recurrent_excitation': self.recurrent_excitation,
                    'inhibitory_weight': self.inhibitory_weight
                }
        
        return ReservoirNeuron(parameters)
    
    def _create_adaptive_neuron(self, parameters: Dict[str, Any]) -> Any:
        """Create an adaptive neuron with plastic threshold"""
        class AdaptiveNeuron:
            def __init__(self, params):
                self.id = id(self)
                self.type = 'adaptive'
                self.base_threshold = params.get('base_threshold', 1.0)
                self.time_constant = params.get('time_constant', 0.015)
                self.input_gain = params.get('input_gain', 1.2)
                self.output_gain = params.get('output_gain', 1.0)
                
                # Adaptation parameters
                self.adaptation_rate = params.get('adaptation_rate', 0.05)
                self.adaptation_decay = params.get('adaptation_decay', 0.98)
                self.threshold_sensitivity = params.get('threshold_sensitivity', 0.8)
                
                # State variables
                self.membrane_potential = 0.0
                self.resting_potential = 0.0
                self.refractory_remaining = 0
                self.firing_count = 0
                self.adaptation = 0.0
                self.input_history = []
                
                # Dynamic threshold
                self.current_threshold = self.base_threshold
            
            def step(self, current_time: float, dt: float) -> bool:
                """Step the adaptive neuron"""
                if self.refractory_remaining > 0:
                    self.refractory_remaining -= 1
                    return False
                
                # Integrate input
                input_current = self.input_gain
                if self.input_history:
                    # Add history-dependent input
                    input_current += sum(self.input_history[-5:]) / len(self.input_history[-5:]) if self.input_history else 0
                
                # LIF dynamics with adaptive threshold
                self.membrane_potential += (self.resting_potential - self.membrane_potential) * dt / self.time_constant
                self.membrane_potential += input_current * dt
                
                # Update dynamic threshold based on adaptation
                self.current_threshold = self.base_threshold * self.threshold_sensitivity + self.adaptation * self.adaptation_rate
                
                # Decay adaptation
                self.adaptation *= self.adaptation_decay
                
                # Check for spike
                if self.membrane_potential >= self.current_threshold:
                    self.membrane_potential = self.resting_potential
                    self.refractory_remaining = 10
                    self.firing_count += 1
                    self.adaptation += 1.0
                    
                    return True
                
                return False
            
            def inject_current(self, current: float) -> None:
                """Inject current into the neuron"""
                self.membrane_potential += current * self.output_gain
                self.input_history.append(current)
                # Keep input history manageable
                if len(self.input_history) > 100:
                    self.input_history.pop(0)
            
            def get_state(self) -> Dict[str, Any]:
                """Get neuron state"""
                return {
                    'id': self.id,
                    'type': self.type,
                    'membrane_potential': self.membrane_potential,
                    'resting_potential': self.resting_potential,
                    'base_threshold': self.base_threshold,
                    'current_threshold': self.current_threshold,
                    'firing_count': self.firing_count,
                    'adaptation': self.adaptation,
                    'input_history': self.input_history
                }
        
        return AdaptiveNeuron(parameters)

# Main plugin class
class ExampleNeuronPlugin(NLMPlugin):
    """Plugin for example neuron extension points"""
    
    def __init__(self):
        super().__init__()
        self.info = PluginInfo(
            name="Example Neuron Plugin",
            version="1.0.0",
            description="Plugin providing custom neuron extension points for NLM",
            author="NLM Team",
            capabilities=["neuron"],
            dependencies=[],
            entry_point="main"
        )
        self.neuron_extension = None
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize plugin with NLM core"""
        if not super().initialize(nlm_core):
            return False
        
        # Create neuron extension point
        self.neuron_extension = ExampleNeuronExtension()
        
        # Initialize the extension point
        if self.neuron_extension.initialize(nlm_core):
            print("Example Neuron Plugin initialized successfully")
            return True
        else:
            print("Failed to initialize Example Neuron Extension")
            return False
    
    def shutdown(self) -> None:
        """Shutdown plugin"""
        if self.neuron_extension:
            self.neuron_extension.shutdown()
            self.neuron_extension = None
        super().shutdown()
        print("Example Neuron Plugin shut down")
    
    def get_neuron_extension(self) -> Optional[ExampleNeuronExtension]:
        """Get the neuron extension point"""
        return self.neuron_extension

# Export plugin class
__all__ = ['ExampleNeuronPlugin', 'ExampleNeuronExtension']
