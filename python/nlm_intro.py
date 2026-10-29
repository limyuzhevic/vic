#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine
An Experimental Artificial Developmental Brain

This script provides a simplified introduction to the NLM neural simulation framework
through examples and basic usage patterns.

The NLM is a brain-inspired computational system that:
- Begins with primitive neural structures
- Learns from experience through synaptic plasticity
- Develops and adapts over time
- Demonstrates emergent cognitive capabilities

Example usage:
    $ python3 nlm_intro.py
"""

import sys
import time
from typing import Dict, List, Any

# Import the NLM interface
try:
    from python.nlm_interface import NLMSimulation, SimulationConfig, BrainTester
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Error importing NLM interface: {e}")
    print("Please ensure you have properly built the NLM Python bindings")
    sys.exit(1)
def print_header(title: str, subtitle: str = ""):
    """Print formatted header."""
    print("\n" + "="*70)
    print(f"  {title}")
    if subtitle:
        print(f"  {subtitle}")
    print("="*70)

def demonstrate_basic_usage():
    """Demonstrate basic NLM usage with example code."""
    print_header("BASIC NLM USAGE", "Getting Started with Neural Simulation")
    
    print("""
# Create a new simulation with Python-friendly configuration:
brain = NLMSimulation(
    neuron_count=500,           # Number of neurons
    random_seed=42,             # For reproducible experiments
    simulation_timestep=0.001,  # Time step in seconds
    region_count=2,             # Number of brain regions
)

# Initialize the brain (sets up neural connectivity and dynamics)
brain.initialize()
print("Brain initialized with", brain.get_status()['total_neurons'], "neurons")

# Run simulation for many steps
print("\\nRunning simulation for 1000 steps...")
results = brain.run(1000)
print(f"Steps completed: {results['steps_completed']}")
print(f"Total spikes generated: {results['total_spikes']}")
print(f"Learning detected: {results['learning_detected']}")

# Check brain status
status = brain.get_status()
print(f"\\nBrain status:")
print(f"  Total neurons: {status['total_neurons']}")
print(f"  Total spikes: {status['total_spikes']}")
print(f"  Firing neurons: {status['firing_neurons']}")
print(f"  Total synapses: {status['total_synapses']}")
print(f"  Average firing rate: {status['average_firing_rate']:.2f} Hz")
""")

def demonstrate_plasticity_learning():
    """Demonstrate synaptic plasticity and learning."""
    print_header("PLASTICITY AND LEARNING", "How NLM Adapts Through Experience")
    
    print("""
# Create simulation with plasticity enabled
brain = NLMSimulation(
    neuron_count=200,
    random_seed=123,
    connection_probability=0.2,  # Higher connectivity for learning
)

brain.initialize()

# Record initial state
initial_status = brain.get_status()
print(f"Initial state - Total spikes: {initial_status['total_spikes']}")

# Run extended simulation to allow learning
print("\\nRunning extended simulation (5000 steps)...")
results = brain.run(5000)

# Record final state
final_status = brain.get_status()
print(f"Final state - Total spikes: {final_status['total_spikes']}")
print(f"Learning detected: {results['learning_detected']}")
print(f"Weight changes occurred: {results['total_synapses'] > 0}")

# Check excitation/inhibition balance
if 'excitation_inhibition_ratio' in final_status:
    e_i_ratio = final_status['excitation_inhibition_ratio']
    print(f"Excitation/Inhibition ratio: {e_i_ratio:.2f}")
""")

def demonstrate_stdp():
    """Demonstrate Spike-Timing-Dependent Plasticity."""
    print_header("STDP - Spike-Timing-Dependent Plasticity", "Learning Through Precise Timing")
    
    print("""
# STDP is a key learning mechanism in NLM where:
# - Simultaneous pre- and post-synaptic spikes → LTP (long-term potentiation)
# - Pre-synaptic before post-synaptic → LTD (long-term depression)

brain = NLMSimulation(
    neuron_count=100,
    random_seed=456,
)

brain.initialize()

# Enable specific plasticity rules
print("\\nEnabling STDP and Hebbian plasticity...")
# In a real implementation, you would configure plasticity rules
# For now, the simulation includes these by default

# Run simulation with correlated activity patterns
print("Running simulation with correlated activity patterns...")
results = brain.run(2000)

print(f"\\nSTDP test results:")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Learning detected: {results['learning_detected']}")
print(f"  Average firing rate: {results['max_firing_rate']:.2f} Hz")

print("\\nSTDP enables the brain to:")
print("  - Strengthen connections for repeated co-activation")
print("  - weaken connections for uncorrelated activity")
print("  - create efficient neural representations")
print("  - support memory formation through synaptic changes")
""")

def demonstrate_configuration():
    """Demonstrate configuration options."""
    print_header("CONFIGURATION OPTIONS", "Customizing NLM Behavior")
    
    print("""
# Create custom configurations for different experimental conditions

# Fast simulation (for testing)
fast_config = SimulationConfig(
    neuron_count=100,
    simulation_timestep=0.01,      # Larger time step
    max_simulation_steps=1000,
)

# High plasticity (for rapid learning)
plasticity_config = SimulationConfig(
    neuron_count=500,
    connection_probability=0.3,    # More connections
    plasticity_learning_rate=0.05, # Higher learning rate
    stdp_ltp_weight=0.05,
    stdp_ltd_weight=0.04,
)

# Developmental simulation
development_config = SimulationConfig(
    neuron_count=200,
    region_count=3,                # Multiple brain regions
    development_synaptogenesis_rate=0.005,  # More synapse formation
    development_pruning_rate=0.001,         # More pruning
)

# Each config can be used with NLMSimulation
# brain = NLMSimulation(config=my_config)
""")

def demonstrate_testing():
    """Demonstrate testing capabilities."""
    print_header("TESTING AND VERIFICATION", "Quality Assurance for NLM")
    
    print("""
# NLM includes comprehensive test suites to verify functionality

brain = NLMSimulation(neuron_count=100, random_seed=789)
brain.initialize()

# Run all standard tests
print("Running comprehensive test suite...")
test_results = BrainTester.run_all_tests(brain)

print(f"\\nTest Results:")
print(f"  Connectivity test: {'PASS' if test_results['connectivity']['connectivity_verified'] else 'FAIL'}")
print(f"  Plasticity test: {'PASS' if test_results['plasticity']['learning_detected'] else 'FAIL'}")
print(f"  STDP test: {'PASS' if test_results['stdp']['stdp_working'] else 'FAIL'}")
print(f"  All tests passed: {test_results['all_passed']}")

# Individual test functions are also available:
# BrainTester.run_basic_connectivity_test(brain)
# BrainTester.run_plasticity_test(brain)
# BrainTester.run_stdp_test(brain)
""")

def demonstrate_sensory_integration():
    """Demonstrate sensory processing and integration."""
    print_header("SENSORY PROCESSING", "Perceiving the Environment")
    
    print("""
# NLM processes various sensory inputs:

sensory_inputs = {
    'vision': [0.1, 0.3, 0.7, 0.9, 0.2],      # Visual field
    'auditory': [0.0, 0.5, 0.0, 0.8, 0.0],   # Sound levels
    'touch': [0.3, 0.1, 0.4],                 # Tactile sensations
    'internal': [0.5, 0.2, 0.1],              # Internal signals
}

brain = NLMSimulation(neuron_count=150)
brain.initialize()

# Add sensory input (in real usage, this would come from environment)
# brain.add_sensory_input(sensory_inputs)

print("\\nNLM processes sensory inputs through:")
print("  - SensoryInput classes (Vision, Audio, InternalSignals)")
print("  - Neural transduction and encoding")
print("  - Population coding for different modalities")
print("  - Integration with working memory")
print("  - Modulation of attention and exploration")
""")

def demonstrate_motor_output():
    """Demonstrate motor action production."""
    print_header("MOTOR OUTPUT", "Translating Neural Activity to Action")
    
    print("""
# NLM generates motor commands based on neural activity

brain = NLMSimulation(neuron_count=120)
brain.initialize()

# Simulate some neural activity (in real usage, this would come from simulation)
# brain.step()  # Would update neural state

# Generate action based on current neural state
action = brain.produce_action()

if action:
    print(f"Generated action:")
    print(f"  Type: {action['action_type']}")
    print(f"  Parameters: {action['parameters']}")
    print(f"  Confidence: {action['confidence']:.2f}")
else:
    print("No action generated (would require neural activity)")

print("\\nMotor system components:")
print("  - Action class: Represents specific actions")
print("  - MotorSystem: Neural-to-action conversion")
print("  - Neural planner: Predictive action selection")
print("  - Integration with environment world")
""")

def demonstrate_neuromodulation():
    """Demonstrate neuromodulatory systems."""
    print_header("NEUROMODULATION", "Motivational and Regulatory Signals")
    
    print("""
# Neuromodulators provide the brain with guidance and motivation

brain = NLMSimulation(neuron_count=180)
brain.initialize()

# Different neuromodulators serve different functions:

# Dopamine: Reward prediction and reinforcement
# - Increases synaptic strength for successful actions
# - Guides learning based on prediction errors
# - Focuses attention on rewarding stimuli

# Curiosity: Exploration motivation
# - Drives exploration of novel stimuli
# - Balances exploitation vs exploration
# - Increases with unexpected events

# Novelty: Change detection
# - Responds to new or unexpected inputs
# - Triggers attention and learning
# - Helps identify important events

# Prediction Error: Learning signal
# - Compares prediction with actual outcome
# - Drives plasticity adjustments
# - Signals where learning is needed

# Apply neuromodulatory signals (in real usage, these would come from environment)
# brain.apply_neuromodulation({'dopamine': 0.8, 'curiosity': 0.5})
""")

def main_demo():
    """Main demonstration function."""
    print_header("NLM (熙然) - Neural Learning Machine", "An Experimental Artificial Developmental Brain")
    
    print("""
Welcome to the NLM neural simulation framework!

NLM is a computational brain project that investigates how neural systems
emerge from simple building blocks through experience and development.

Key Features:
├─ Real spiking neural computation (LIF neurons)
├─ Event-driven spike propagation with delays
├─ Multiple plasticity rules (STDP, Hebbian, reward-modulated)
├─ Structural plasticity (synaptogenesis/pruning)
├─ Memory systems (working, episodic, associative)
├─ Prediction and planning capabilities
├─ Neuromodulation (dopamine, curiosity, novelty)
├─ Developmental stages and maturation
├─ Sensory processing (vision, audio, internal)
└─ Motor action production

This framework is designed to:
1. Provide a biologically-inspired neural substrate
2. Investigate emergent cognitive capabilities
3. Test hypotheses about brain function
4. Serve as a platform for AI research
5. Enable reproducible neural experiments
""")
    
    print("To get started with a simple example:")
    print("  python3 nlm_intro.py --demo <example_type>")
    print("\nAvailable demo types:")
    print("  basic     - Basic NLM usage and setup")
    print("  plasticity - Synaptic plasticity and learning")
    print("  stdp      - Spike-Timing-Dependent Plasticity")
    print("  config    - Configuration options")
    print("  testing   - Testing and verification")
    print("  sensory   - Sensory processing")
    print("  motor     - Motor output generation")
    print("  neuromod   - Neuromodulation systems")
    print("  all       - Run all demonstrations")

def parse_arguments():
    """Parse command line arguments."""
    if len(sys.argv) < 2:
        main_demo()
        return None, False
    
    demo_type = sys.argv[1].lower()
    show_help = sys.argv[2] == "--help" if len(sys.argv) > 2 else False
    
    if show_help or demo_type == "help":
        main_demo()
        return None, True
    
    return demo_type, False
def run_demo(demo_type: str):
    """Run specific demonstration."""
    demo_functions = {
        'basic': demonstrate_basic_usage,
        'plasticity': demonstrate_plasticity_learning,
        'stdp': demonstrate_stdp,
        'config': demonstrate_configuration,
        'testing': demonstrate_testing,
        'sensory': demonstrate_sensory_integration,
        'motor': demonstrate_motor_output,
        'neuromod': demonstrate_neuromodulation,
        'all': lambda: [
            demonstrate_basic_usage(),
            demonstrate_plasticity_learning(),
            demonstrate_stdp(),
            demonstrate_configuration(),
            demonstrate_testing(),
            demonstrate_sensory_integration(),
            demonstrate_motor_output(),
            demonstrate_neuromodulation(),
        ]
    }
    
    if demo_type in demo_functions:
        result = demo_functions[demo_type]()
        if isinstance(result, list):
            for func_result in result:
                if func_result:
                    time.sleep(0.5)  # Brief pause between demos
    else:
        print(f"Unknown demo type: {demo_type}")
        print("Available types: " + ", ".join(demo_functions.keys()))
        return False
    
    return True
if __name__ == "__main__":
    demo_type, show_help = parse_arguments()
    
    if show_help:
        sys.exit(0)
    
    if demo_type:
        success = run_demo(demo_type)
        if not success:
            sys.exit(1)
    else:
        # Show all demos in sequence
        print("\nRunning all demonstrations...")
        all_demos = lambda: [
            demonstrate_basic_usage(),
            demonstrate_plasticity_learning(),
            demonstrate_stdp(),
            demonstrate_configuration(),
            demonstrate_testing(),
            demonstrate_sensory_integration(),
            demonstrate_motor_output(),
            demonstrate_neuromodulation(),
        ]
        
        all_demos()
        
        print_header("DEMONSTRATION COMPLETE", "Thank you for exploring NLM!")
        print("\nThe NLM framework provides a powerful platform for neural simulation.")
        print("For more advanced usage, explore the Python interface or C++ API.")
