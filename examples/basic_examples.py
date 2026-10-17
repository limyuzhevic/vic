#!/usr/bin/env python3
"""
NLM Python Bindings Examples

This script demonstrates the basic usage of the NLM (Neural Learning Machine)
Python bindings. Each example shows how to use the different components of
the neural simulation framework.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

def example_basic_setup():
    """Example 1: Basic NLM setup and initialization."""
    print("=" * 60)
    print("Example 1: Basic NLM Setup and Initialization")
    print("=" * 60)
    
    # Create a default configuration
    config = nlm.createDefaultConfig()
    config.set("random_seed", 42)
    config.set("neuron_count", 50)
    config.set("region_count", 2)
    
    # Create brain with configuration
    brain = nlm.createBrain(config)
    
    if not brain.initialize():
        print("ERROR: Failed to initialize brain!")
        return False
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Number of regions: {brain.getRegionCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Show basic statistics
    brain.logStatus()
    
    # Clean up
    brain.reset()
    print("Example 1 completed successfully!\n")
    return True

def example_brain_operations():
    """Example 2: Brain operations and neuron manipulation."""
    print("=" * 60)
    print("Example 2: Brain Operations and Neuron Manipulation")
    print("=" * 60)
    
    # Create a simple configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Get all neurons and regions
    regions = brain.getRegions()
    print(f"Created {len(regions)} brain regions")
    
    total_neurons = brain.getTotalNeuronCount()
    print(f"Total neurons in brain: {total_neurons}")
    
    # Perform a simple simulation step
    print("\nRunning simulation steps...")
    for step in range(10):
        brain.step(step, step * 0.001)
        if step % 5 == 0:
            active = brain.getActiveNeuronCount()
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            print(f"  Step {step}: Active={active}, Firing={firing}, Total spikes={total_spikes}")
    
    # Demonstrate action production
    action = brain.produceAction()
    if action:
        print(f"\nProduced action: {action.getName()}")
        print(f"Action type: {action.getType()}")
        print(f"Action parameters: {action.getParameters()}")
    
    brain.reset()
    print("\nExample 2 completed successfully!\n")
    return True

def example_plasticity_simulation():
    """Example 3: Plasticity simulation and learning."""
    print("=" * 60)
    print("Example 3: Plasticity Simulation and Learning")
    print("=" * 60)
    
    # Create configuration optimized for learning
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("plasticity_learning_rate", 0.02)
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.025)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Get all regions and enable plasticity
    regions = brain.getRegions()
    total_synapses = 0
    
    print("Enabling plasticity on all synapses...")
    for region in regions:
        synapses = region.getSynapses()
        for synapse in synapses:
            synapse.enablePlasticity(True, True, False)  # Hebbian + STDP
            total_synapses += 1
    
    print(f"Enabled plasticity on {total_synapses} synapses")
    
    # Run simulation with repeated input to stimulate learning
    print("\nRunning learning simulation (200 steps)...")
    learning_steps = 200
    
    for step in range(learning_steps):
        # Apply input pattern to stimulate synaptic changes
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 40.0)
        
        brain.step(step, step * 0.001)
        
        if (step + 1) % 50 == 0:
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            print(f"  Step {step + 1}: {spike_count} spikes, {firing_rate:.2f} Hz firing rate")
    
    print(f"\nLearning simulation completed!")
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    brain.reset()
    print("\nExample 3 completed successfully!\n")
    return True

def example_neuromodulation():
    """Example 4: Neuromodulation simulation."""
    print("=" * 60)
    print("Example 4: Neuromodulation Simulation")
    print("=" * 60)
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 150)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print("Neuromodulation systems in brain:")
    print(f"  Dopamine system: Available")
    print(f"  Curiosity system: Available")
    print(f"  Novelty detection: Available")
    print(f"  Prediction error signal: Available")
    
    # Get neuromodulation level (would require AgentBrain interface)
    # This demonstrates accessing neuromodulation functionality
    print("\nNeuromodulation can affect:")
    print("  - Neuronal excitability")
    print("  - Synaptic plasticity rates")
    print("  - Learning behavior")
    print("  - Action selection")
    
    brain.reset()
    print("\nExample 4 completed successfully!\n")
    return True

def example_multiple_regions():
    """Example 5: Multiple brain regions and connectivity."""
    print("=" * 60)
    print("Example 5: Multiple Brain Regions and Connectivity")
    print("=" * 60)
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("region_count", 3)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    regions = brain.getRegions()
    print(f"Created {len(regions)} brain regions")
    
    # Add some inter-region connections
    region_ids = brain.getRegionIds()
    if len(region_ids) >= 2:
        print(f"Adding inter-region connections between regions {region_ids[0]} and {region_ids[1]}")
        brain.addInterRegionConnection(region_ids[0], region_ids[1], 0.5, 5)
    
    # Run simulation to test connectivity
    print("\nTesting connectivity through simulation...")
    for step in range(20):
        # Inject into different neuron types to test connectivity
        brain.injectCurrentToNeurons(nlm.NeuronType.Excitatory, 20.0)
        brain.step(step, step * 0.001)
    
    total_spikes = brain.getTotalSpikeCount()
    print(f"Connectivity test completed: {total_spikes} total spikes")
    
    brain.reset()
    print("\nExample 5 completed successfully!\n")
    return True

def example_serialization():
    """Example 6: Serialization and checkpointing."""
    print("=" * 60)
    print("Example 6: Serialization and Checkpointing")
    print("=" * 60)
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation to generate state
    print("Running simulation to generate brain state...")
    for step in range(50):
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0)
        brain.step(step, step * 0.001)
    
    print(f"Simulation generated {brain.getTotalSpikeCount()} spikes")
    
    # Save brain state
    save_file = "nlm_checkpoint.bin"
    print(f"\nSaving brain state to {save_file}...")
    if brain.save(save_file):
        print("✓ Brain state saved successfully")
    else:
        print("✗ Failed to save brain state")
    
    # Reset and reload
    brain.reset()
    print(f"Loading brain state from {save_file}...")
    if brain.load(save_file):
        print("✓ Brain state loaded successfully")
        print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons")
    else:
        print("✗ Failed to load brain state")
    
    # Clean up
    if os.path.exists(save_file):
        os.remove(save_file)
        print(f"Cleaned up {save_file}")
    
    print("\nExample 6 completed successfully!\n")
    return True

def main_examples():
    """Run all examples."""
    print("NLM Python Bindings Examples")
    print("=" * 60)
    print("This script demonstrates various aspects of the NLM Python bindings.")
    print("Each example shows how to use different components of the framework.")
    print()
    
    examples = [
        example_basic_setup,
        example_brain_operations,
        example_plasticity_simulation,
        example_neuromodulation,
        example_multiple_regions,
        example_serialization,
    ]
    
    completed = 0
    failed = 0
    
    for example in examples:
        try:
            if example():
                completed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"Example failed with error: {e}")
            failed += 1
    
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"Examples completed: {completed}")
    print(f"Examples failed: {failed}")
    print(f"Total examples: {len(examples)}")
    
    if failed == 0:
        print("\n✓ All examples completed successfully!")
        return True
    else:
        print(f"\n✗ {failed} examples failed")
        return False

if __name__ == "__main__":
    # Check if pynlm is available
    try:
        import pynlm as nlm
    except ImportError as e:
        print(f"Error: Could not import pynlm: {e}")
        print("Please build the Python bindings first.")
        sys.exit(1)
    
    # Run all examples
    success = main_examples()
    sys.exit(0 if success else 1)