#!/usr/bin/env python3
"""
Simple command-line interface for the NLM (Neural Learning Machine) library.

This script provides a basic interface to the NLM neural simulation framework,
demonstrating core functionality such as brain initialization, simulation steps,
and basic interaction with the neural system.
"""

import sys
import time
import os
from pathlib import Path

# Add the source directory to the Python path
sys.path.insert(0, str(Path(__file__).parent / "python"))

try:
    import pynlm as nlm
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please build the Python bindings first.")
    sys.exit(1)

def print_header():
    print("=" * 60)
    print("NLM - Neural Learning Machine - Python CLI Interface")
    print("=" * 60)
    print()

def print_help():
    print("Usage: nlm-cli [OPTIONS]")
    print()
    print("Options:")
    print("  --help                    Show this help message")
    print("  --config FILE             Load configuration from FILE")
    print("  --steps STEPS             Number of simulation steps (default: 100)")
    print("  --neuron-count COUNT      Number of neurons (default: 100)")
    print("  --seed SEED               Random seed (default: 42)")
    print("  --plasticity              Enable plasticity for the simulation")
    print("  --verbose                 Enable verbose output")
    print("  --version                 Show version information")
    print()
    print("Example:")
    print("  nlm-cli --steps 500 --neuron-count 200 --seed 123")

def create_default_config(neuron_count=100, random_seed=42):
    """Create a default configuration for NLM simulation."""
    config = nlm.createDefaultConfig()
    
    # Set basic configuration
    config.set("random_seed", random_seed)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    config.set("connection_probability", 0.15)
    
    # Set plasticity parameters if requested
    config.set("plasticity_learning_rate", 0.01)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    config.set("stdp_time_constant", 20.0)
    
    # Set development parameters
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    
    return config

def run_basic_simulation(config, steps=100, verbose=False):
    """Run a basic NLM simulation."""
    print(f"Initializing NLM brain with {config.getOr('neuron_count', 100)} neurons...")
    
    # Create brain
    brain = nlm.createBrain(config)
    
    if not brain.initialize():
        print("ERROR: Failed to initialize brain!")
        return False
    
    brain.logStatus()
    
    print(f"\nRunning {steps} simulation steps...")
    
    # Run simulation
    for step in range(steps):
        brain.step(step, step * config.getOr('simulation_timestep', 0.001))
        
        if verbose and step % 50 == 0:
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            print(f"  Step {step}: Spikes={spike_count}, Firing rate={firing_rate:.2f} Hz")
    
    # Get final statistics
    print(f"\nSimulation completed!")
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return True

def run_learning_experiment(config, steps=1000, verbose=False):
    """Run a plasticity learning experiment."""
    print(f"\nInitializing learning experiment...")
    
    # Create brain
    brain = nlm.createBrain(config)
    
    if not brain.initialize():
        print("ERROR: Failed to initialize brain!")
        return False
    
    # Enable plasticity on all synapses
    regions = brain.getRegions()
    for region in regions:
        for synapse in region.getSynapses():
            synapse.enablePlasticity(True, True, False)  # Hebbian and STDP
    
    print("Plasticity enabled on all synapses.")
    
    # Apply repeated input pattern to stimulate learning
    print(f"Applying repeated input patterns ({steps} steps)...")
    
    for step in range(steps):
        # Create input pattern - inject current into sensory neurons
        neurons_count = brain.getTotalNeuronCount()
        for i in range(min(20, neurons_count // 4)):
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0)
        
        brain.step(step, step * config.getOr('simulation_timestep', 0.001))
        
        if verbose and step % 200 == 0:
            spike_count = brain.getTotalSpikeCount()
            firing_count = brain.getFiringNeuronCount()
            print(f"  Step {step}: Spikes={spike_count}, Active firing neurons={firing_count}")
    
    print(f"\nLearning experiment completed!")
    brain.logStatus()
    
    return True

def run_plasticity_demo(config, verbose=False):
    """Run a simple plasticity demonstration."""
    print(f"\nRunning plasticity demonstration...")
    
    # Create brain
    brain = nlm.createBrain(config)
    
    if not brain.initialize():
        print("ERROR: Failed to initialize brain!")
        return False
    
    # Get the first region for demonstration
    regions = brain.getRegions()
    if not regions:
        print("ERROR: No regions found in brain!")
        return False
    
    region = regions[0]
    neurons = region.getAllNeurons()
    
    if len(neurons) < 2:
        print("ERROR: Not enough neurons for demonstration!")
        return False
    
    # Record initial weights
    synapses = region.getSynapses()
    if synapses:
        initial_weights = []
        for synapse in synapses[:5]:  # Test first 5 synapses
            initial_weights.append(synapse.getWeight())
        
        print(f"Initial weights (first 5 synapses): {initial_weights}")
        
        # Enable plasticity for demonstration
        for synapse in synapses:
            synapse.enablePlasticity(False, True, False)  # STDP only
        
        print("Enabled STDP plasticity (learning only through spike timing).")
        
        # Create correlated activity to trigger LTP
        print("Creating correlated pre->post activity (50 trials for potentiation)...")
        
        for trial in range(50):
            # Fire pre-synaptic neuron
            pre_neuron = neurons[0]
            pre_neuron.injectCurrent(60.0)
            brain.step(trial * 2, trial * 2 * config.getOr('simulation_timestep', 0.001))
            
            # Then post fires
            post_neuron = neurons[1]
            post_neuron.injectCurrent(60.0)
            brain.step(trial * 2 + 1, (trial * 2 + 1) * config.getOr('simulation_timestep', 0.001))
        
        # Check weight changes
        final_weights = []
        for synapse in synapses[:5]:
            final_weights.append(synapse.getWeight())
        
        print(f"Final weights (first 5 synapses): {final_weights}")
        
        # Calculate changes
        weight_changes = [final - initial for final, initial in zip(final_weights, initial_weights)]
        total_change = sum(weight_changes)
        
        print(f"Weight changes: {weight_changes}")
        print(f"Total change: {total_change:.4f}")
        
        if total_change > 0.001:
            print("✓ STDP WORKING: Pre-before-post produced potentiation (LTP)")
        elif total_change < -0.001:
            print("! STDP reversed: Check parameters")
        else:
            print("! No change: STDP may not be triggering")
    
    return True

def main():
    # Parse command line arguments
    config_file = None
    steps = 100
    neuron_count = 100
    random_seed = 42
    enable_plasticity = False
    verbose = False
    
    i = 1
    while i < len(sys.argv):
        arg = sys.argv[i]
        
        if arg == "--help":
            print_help()
            return 0
        elif arg == "--version":
            print("NLM Python CLI v0.1.0")
            print("Built with pybind11 for Python bindings")
            return 0
        elif arg == "--config" and i + 1 < len(sys.argv):
            config_file = sys.argv[i + 1]
            i += 1
        elif arg == "--steps" and i + 1 < len(sys.argv):
            steps = int(sys.argv[i + 1])
            i += 1
        elif arg == "--neuron-count" and i + 1 < len(sys.argv):
            neuron_count = int(sys.argv[i + 1])
            i += 1
        elif arg == "--seed" and i + 1 < len(sys.argv):
            random_seed = int(sys.argv[i + 1])
            i += 1
        elif arg == "--plasticity":
            enable_plasticity = True
        elif arg == "--verbose":
            verbose = True
        elif arg.startswith("--"):
            print(f"Unknown option: {arg}")
            print_help()
            return 1
        else:
            print(f"Unknown argument: {arg}")
            print_help()
            return 1
        
        i += 1
    
    print_header()
    
    # Create configuration
    config = create_default_config(neuron_count, random_seed)
    
    # Load config file if specified
    if config_file and Path(config_file).exists():
        print(f"Loading configuration from: {config_file}")
        if not config.loadFromFile(config_file):
            print(f"Warning: Could not load config from {config_file}, using defaults")
    
    # Run different types of demonstrations
    success = True
    
    # 1. Basic connectivity test
    print("\n" + "="*60)
    print("1. BASIC NEURAL CONNECTIVITY TEST")
    print("="*60)
    success &= run_basic_simulation(config, steps, verbose)
    
    # 2. Learning experiment (if plasticity enabled)
    if enable_plasticity:
        print("\n" + "="*60)
        print("2. PLASTICITY LEARNING EXPERIMENT")
        print("="*60)
        success &= run_learning_experiment(config, 1000, verbose)
    
    # 3. STDP verification
    print("\n" + "="*60)
    print("3. STDP PLASTICITY VERIFICATION")
    print("="*60)
    success &= run_plasticity_demo(config, verbose)
    
    if success:
        print("\n" + "="*60)
        print("ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY!")
        print("="*60)
        return 0
    else:
        print("\n" + "="*60)
        print("ERROR: SOME DEMONSTRATIONS FAILED!")
        print("="*60)
        return 1

if __name__ == "__main__":
    sys.exit(main())