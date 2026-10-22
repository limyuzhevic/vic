# NLM Python Package - Main Interface
# This file serves as the main Python module interface for NLM

__version__ = "0.1.0"
__author__ = "NLM Project" 
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import everything from the pynlm submodule
from .pynlm import *

# Create a simple Python interface class for easier usage
class NLMInterface:
    """High-level interface for NLM operations."""
    
    def __init__(self, config_dict=None):
        """Initialize NLM interface with optional configuration."""
        from .pynlm import createDefaultConfig, createBrain
        
        self.config = createDefaultConfig()
        
        if config_dict:
            for key, value in config_dict.items():
                if isinstance(value, bool):
                    self.config.set(key, value)
                elif isinstance(value, int):
                    self.config.set(key, value)
                elif isinstance(value, float):
                    self.config.set(key, value)
                else:
                    self.config.set(key, value)
        
        self.brain = createBrain(self.config)
        self.brain.initialize()
    
    def run(self, steps=1000):
        """Run simulation for given number of steps."""
        for i in range(steps):
            self.brain.step(i)
        return self
    
    def get_stats(self):
        """Get brain statistics."""
        return {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'firing': self.brain.getFiringNeuronCount(),
            'spikes': self.brain.getTotalSpikeCount(),
            'avg_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio()
        }

# Create a global convenience function
def create_brain_with_config(config_dict=None):
    """Convenience function to create a brain with configuration."""
    from .pynlm import createDefaultConfig, createBrain
    
    config = createDefaultConfig()
    
    if config_dict:
        for key, value in config_dict.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            else:
                config.set(key, value)
    
    brain = createBrain(config)
    brain.initialize()
    return brain

# Re-export all symbols from the pynlm module
__all__ = list(locals().keys())