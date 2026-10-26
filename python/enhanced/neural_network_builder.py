"""
Neural Network Builder for NLM

This module provides utilities for creating and configuring neural networks
for NLM systems, including specialized network types and developmental networks.
"""

import sys
from pathlib import Path

# Add parent directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

class NeuralNetworkBuilder:
    """
    Builder for creating neural networks for NLM systems.
    
    This class provides methods for creating:
    - Basic neural networks
    - Sensory-motor networks
    - Developmental networks
    - Specialized network configurations
    """
    
    def create_basic_network(self, neuron_count, region_count):
        """
        Create a basic neural network.
        
        Args:
            neuron_count: Number of neurons in the network
            region_count: Number of brain regions
            
        Returns:
            Brain object or None if creation failed
        """
        try:
            from pynlm_enhanced import NeuralNetworkBuilder as EnhancedBuilder
            return EnhancedBuilder.createBasicNetwork(neuron_count, region_count)
        except ImportError:
            # Fallback implementation
            try:
                from pynlm import createDefaultConfig, createBrain
                
                config = createDefaultConfig()
                config.set("neuron_count", neuron_count)
                config.set("region_count", region_count)
                
                brain = createBrain(config)
                brain.initialize()
                return brain
            except ImportError:
                raise ImportError("NLM bindings not available")
    
    def create_sensory_motor_network(self, sensory_neurons, motor_neurons, internal_neurons):
        """
        Create a sensory-motor neural network.
        
        Args:
            sensory_neurons: Number of sensory neurons
            motor_neurons: Number of motor neurons
            internal_neurons: Number of internal neurons
            
        Returns:
            Brain object or None if creation failed
        """
        try:
            from pynlm_enhanced import NeuralNetworkBuilder as EnhancedBuilder
            return EnhancedBuilder.createSensoryMotorNetwork(sensory_neurons, motor_neurons, internal_neurons)
        except ImportError:
            # Fallback implementation
            try:
                from pynlm import createDefaultConfig, createBrain
                
                config = createDefaultConfig()
                total_neurons = sensory_neurons + motor_neurons + internal_neurons
                config.set("neuron_count", total_neurons)
                config.set("region_count", 3)
                config.set("connection_probability", 0.15f)
                
                brain = createBrain(config)
                brain.initialize()
                return brain
            except ImportError:
                raise ImportError("NLM bindings not available")
    
    def create_developmental_network(self, base_neuron_count):
        """
        Create a developmental neural network.
        
        Args:
            base_neuron_count: Base number of neurons
            
        Returns:
            Brain object or None if creation failed
        """
        try:
            from pynlm_enhanced import NeuralNetworkBuilder as EnhancedBuilder
            return EnhancedBuilder.createDevelopmentalNetwork(base_neuron_count)
        except ImportError:
            # Fallback implementation
            try:
                from pynlm import createDefaultConfig, createBrain
                
                config = createDefaultConfig()
                config.set("neuron_count", base_neuron_count)
                config.set("region_count", 2)
                config.set("connection_probability", 0.12f)
                config.set("synaptogenesis_rate", 0.001f)
                config.set("pruning_rate", 0.0001f)
                
                brain = createBrain(config)
                brain.initialize()
                return brain
            except ImportError:
                raise ImportError("NLM bindings not available")
    
    # Create standalone functions for backward compatibility
    def create_basic_network_func(self, neuron_count, region_count):
        """Create a basic network (function version)"""
        return self.create_basic_network(neuron_count, region_count)
    
    def create_sensory_motor_network_func(self, sensory_neurons, motor_neurons, internal_neurons):
        """Create a sensory-motor network (function version)"""
        return self.create_sensory_motor_network(sensory_neurons, motor_neurons, internal_neurons)
    
    def create_developmental_network_func(self, base_neuron_count):
        """Create a developmental network (function version)"""
        return self.create_developmental_network(base_neuron_count)
    
    # Alias for backward compatibility
    createBasicNetwork = create_basic_network_func
    createSensoryMotorNetwork = create_sensory_motor_network_func
    createDevelopmentalNetwork = create_developmental_network_func

# Create a default instance
_default_builder = NeuralNetworkBuilder()

# Convenience functions for backward compatibility
def create_basic_network(neuron_count, region_count):
    return _default_builder.create_basic_network(neuron_count, region_count)

def create_sensory_motor_network(sensory_neurons, motor_neurons, internal_neurons):
    return _default_builder.create_sensory_motor_network(sensory_neurons, motor_neurons, internal_neurons)

def create_developmental_network(base_neuron_count):
    return _default_builder.create_developmental_network(base_neuron_count)

# Set module attributes for backward compatibility
__all__ = [
    'NeuralNetworkBuilder',
    'create_basic_network',
    'create_sensory_motor_network', 
    'create_developmental_network'
]

# For use in enhanced package
builder = _default_builder