# python/pynlm/_core.py
"""
Core improved functionality for NLM Python bindings
"""

from typing import Dict, List, Optional, Union, Any, Callable
import numpy as np

# Import the original C++ bindings
import pynlm

# Re-export the original functionality
__all__ = [
    'createDefaultConfig',
    'createBrain', 
    'createSimpleWorld',
    'createAgentBrain',
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'NeuronType',
    'SynapseType',
    'ActionType',
    'DevelopmentalStage',
    'FiringState',
    'MotorCommand',
    'WorldObjectType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID'
]

# Create aliases for original pynlm classes
for attr in __all__:
    if hasattr(pynlm, attr):
        locals()[attr] = getattr(pynlm, attr)

# Improved convenience functions
def create_simulation_config(neuron_count: int = 1000, region_count: int = 1) -> 'Config':
    """Create a simulation configuration with sensible defaults."""
    config = createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", 0.1)
    return config

def run_brain_simulation(brain: 'Brain', num_steps: int, enable_logging: bool = False) -> Dict[str, Any]:
    """Run a brain simulation with monitoring."""
    stats = {
        'total_spikes': 0,
        'average_firing_rate': 0.0,
        'max_firing_rate': 0.0,
        'min_firing_rate': float('inf'),
        'firing_rates': []
    }
    
    for step in range(num_steps):
        firing_rate = brain.getAverageFiringRate()
        stats['firing_rates'].append(firing_rate)
        stats['max_firing_rate'] = max(stats['max_firing_rate'], firing_rate)
        stats['min_firing_rate'] = min(stats['min_firing_rate'], firing_rate)
        brain.step(step)
        stats['total_spikes'] = brain.getTotalSpikeCount()
        
        if enable_logging:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"rate={firing_rate:.2f} Hz")
    
    if stats['firing_rates']:
        stats['average_firing_rate'] = np.mean(stats['firing_rates'])
        stats['min_firing_rate'] = stats['min_firing_rate'] if stats['min_firing_rate'] != float('inf') else 0.0
    
    return stats

# Add more convenience functions as needed
