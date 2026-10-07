# NLM Python Utilities

import sys
from typing import Dict, Any
from .neural_types import ActionType, NeuronType, DevelopmentalStage

# Try to import matplotlib for plotting
try:
    import matplotlib.pyplot as plt
    _HAS_MATPLOTLIB = True
except ImportError:
    _HAS_MATPLOTLIB = False

def plot_neural_activity(brain, steps: int = 1000, save_path: str = None) -> None:
    """Plot neural activity over time."""
    if not _HAS_MATPLOTLIB:
        print("matplotlib not available. Install with: pip install matplotlib", file=sys.stderr)
        return
    
    # Generate mock data for demonstration
    import random
    time_points = list(range(steps))
    firing_rates = [random.uniform(0, 10) for _ in range(steps)]
    
    plt.figure(figsize=(12, 6))
    plt.subplot(2, 1, 1)
    plt.plot(time_points, firing_rates)
    plt.xlabel('Simulation Step')
    plt.ylabel('Firing Rate (Hz)')
    plt.title('Neural Activity Over Time')
    plt.grid(True, alpha=0.3)
    
    # Plot action distribution
    actions = [ActionType(i) for _ in range(steps)]
    action_counts = {}
    for action in actions:
        action_counts[action] = action_counts.get(action, 0) + 1
    
    plt.subplot(2, 1, 2)
    plt.bar(range(len(action_counts)), list(action_counts.values()))
    plt.xlabel('Action Type')
    plt.ylabel('Frequency')
    plt.title('Action Distribution')
    plt.xticks(range(len(action_counts)), list(action_counts.keys()))
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150)
    
    plt.show()

def analyze_performance(brain) -> Dict[str, Any]:
    """Analyze brain performance metrics."""
    return {
        'total_neurons': brain.total_neuron_count,
        'firing_neurons': brain.firing_neuron_count,
        'avg_firing_rate': brain.average_firing_rate,
        'exc_inh_ratio': brain.excitation_inhibition_ratio,
        'developmental_stage': brain.developmental_stage.name,
        'total_spikes': brain._total_spikes,
        'simulation_steps': brain._steps
    }

def demo():
    """Run a simple demonstration."""
    print("=== NLM Python Demo ===")
    
    # Import here to avoid circular imports
    from .core import create_default_config, createBrain
    from .world import createSimpleWorld
    from .agent import createAgentBrain
    
    # Create brain
    config = create_default_config()
    brain = createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.total_neuron_count} neurons")
    print(f"Total synapses: {brain.total_synapse_count}")
    
    # Run simulation
    for i in range(100):
        brain.step(i)
        if i % 20 == 0:
            print(f"Step {i}: {brain.firing_neuron_count} firing neurons, "
                  f"{brain.average_firing_rate:.2f} avg firing rate")
    
    print("\n=== Demo Complete ===")
    print(f"Final stats: {analyze_performance(brain)}")

# Module-level constants and metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Common neuron types for easy access
EXCITATORY = NeuronType.EXCITATORY
INHIBITORY = NeuronType.INHIBITORY
SENSORY = NeuronType.SENSORY
MOTOR = NeuronType.MOTOR
MODULATORY = NeuronType.MODULATORY
INTERNAL = NeuronType.INTERNAL

# Developmental stages
INITIAL = DevelopmentalStage.INITIAL
CRITICAL_PERIOD = DevelopmentalStage.CRITICAL_PERIOD
MATURATION = DevelopmentalStage.MATURATION
ADULT = DevelopmentalStage.ADULT
AGING = DevelopmentalStage.AGING

# Action types
MOVE_FORWARD = ActionType.MOVE_FORWARD
TURN_LEFT = ActionType.TURN_LEFT
TURN_RIGHT = ActionType.TURN_RIGHT
INTERACT = ActionType.INTERACT
EAT = ActionType.EAT
SLEEP = ActionType.SLEEP
EXPLORE = ActionType.EXPLORE
STAY = ActionType.STAY