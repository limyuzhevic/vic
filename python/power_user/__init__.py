"""
NLM Power User Advanced Functionality System

This module provides comprehensive advanced functionality for power users of the NLM Python bindings.

For detailed documentation, see the README.md file in the power_user directory.
"""

from .power_user_interface import PowerUserInterface, PowerUserError
from .safety import SafetyManager, OperationMode

__all__ = [
    # Main interfaces
    'PowerUserInterface',
    'PowerUserError',
    
    # Safety and transaction management
    'SafetyManager',
    'OperationMode',
]

# Global power user interface instance
_global_power_user_interface = None

def get_power_user_interface(performance_config=None):
    """Get or create global power user interface"""
    global _global_power_user_interface
    if _global_power_user_interface is None:
        from .power_user_interface import PowerUserInterface
        _global_power_user_interface = PowerUserInterface(None, performance_config)
    return _global_power_user_interface

def export_power_user_examples():
    """Export power user examples and documentation"""
    examples = {
        'advanced_neural_control': {
            'description': 'Direct neuron and synapse manipulation',
            'examples': [
                'Creating custom neuron types with specific properties',
                'Direct synapse weight modification',
                'Neural population management',
                'Circuit-level editing capabilities'
            ]
        },
        'advanced_neuromodulation': {
            'description': 'Custom neuromodulator creation and control',
            'examples': [
                'Creating custom neuromodulators with unique effect functions',
                'Cross-system neuromodulation effects',
                'Neuromodulator interaction modeling'
            ]
        },
        'advanced_memory': {
            'description': 'Memory system with advanced features',
            'examples': [
                'Advanced memory indexing and querying',
                'Memory consolidation control',
                'Cross-modal memory associations',
                'Memory replay and reconstruction'
            ]
        },
        'custom_plasticity': {
            'description': 'User-defined plasticity rules',
            'examples': [
                'Creating custom learning algorithms',
                'Rule composition and chaining',
                'Parameter optimization for plasticity'
            ]
        },
        'simulation_control': {
            'description': 'Advanced simulation control',
            'examples': [
                'Fine-grained simulation timing control',
                'Custom event systems',
                'Multi-threaded simulation control',
                'Simulation state inspection and manipulation'
            ]
        }
    }
    
    return examples

print("Power User Advanced Functionality module loaded successfully!")