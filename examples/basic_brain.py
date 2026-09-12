"""
Basic Brain Example
===================

This example demonstrates the fundamental usage of NLM to create and simulate a neural brain.
It covers the core concepts of brain initialization, stepping, and basic statistics.

Perfect for beginners learning NLM basics.
"""

import sys
import time
from typing import Dict, Any
import nlm

# Type aliases for better readability
SimulationStep = int
TimeStamp = float


def create_simple_brain(neuron_count: int = 100) -> nlm.Brain:
    """
    Create a simple brain with specified neuron count.
    
    Args:
        neuron_count: Number of neurons to create (default: 100)
        
    Returns:
        Initialized Brain object
        
    Raises:
        RuntimeError: If brain creation or initialization fails
    """
    try:
        # Create default configuration
        config = nlm.createDefaultConfig()
        
        # Configure brain parameters
        config.set("neuron_count", neuron_count)
        config.set("random_seed", 42)
        
        # Create and initialize brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        return brain
    except Exception as e:
        raise RuntimeError(f"Failed to create brain: {e}")


def simulate_brain(
    brain: nlm.Brain, 
    num_steps: int = 100, 
    delay: float = 0.01
) -> Dict[str, Any]:
    """
    Run a basic brain simulation for specified number of steps.
    
    Args:
        brain: The brain to simulate
        num_steps: Number of simulation steps to run (default: 100)
        delay: Delay between steps in seconds (default: 0.01)
        
    Returns:
        Dictionary containing simulation statistics
        
    Raises:
        RuntimeError: If simulation fails
    """
    if brain is None:
        raise ValueError("Brain cannot be None")
    
    if num_steps < 1:
        raise ValueError("Number of steps must be at least 1")
    
    try:
        print(f"Starting brain simulation for {num_steps} steps...")
        print(f"Initial state: {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
            
            if delay > 0:
                time.sleep(delay)
            
            # Log progress every 20 steps
            if (step + 1) % 20 == 0:
                print(f"  Step {step + 1}/{num_steps} completed")
        
        # Collect statistics
        stats = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        print(f"\nSimulation completed!")
        print(f"Statistics:")
        print(f"  - Total neurons: {stats['total_neurons']}")
        print(f"  - Total synapses: {stats['total_synapses']}")
        print(f"  - Active neurons: {stats['active_neurons']}")
        print(f"  - Firing neurons: {stats['firing_neurons']}")
        print(f"  - Total spikes: {stats['total_spikes']}")
        print(f"  - Average firing rate: {stats['average_firing_rate']:.2f} Hz")
        print(f"  - E/I ratio: {stats['e_i_ratio']:.3f}")
        print(f"  - Developmental stage: {stats['developmental_stage']}")
        
        return stats
        
    except Exception as e:
        raise RuntimeError(f"Brain simulation failed: {e}")


def demonstrate_neuron_activity(brain: nlm.Brain, steps: int = 50) -> None:
    """
    Demonstrate how neuron activity changes over time.
    
    Args:
        brain: Brain to observe
        steps: Number of steps to simulate and observe
    """
    print("\n" + "="*50)
    print("NEURON ACTIVITY DEMONSTRATION")
    print("="*50)
    
    print(f"Simulating {steps} steps and observing neuron activity...")
    
    for i in range(steps):
        brain.step(i)
        
        if i % 10 == 0:
            firing = brain.getFiringNeuronCount()
            active = brain.getActiveNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"  Step {i+1:3d}: {firing:3d} firing, {active:3d} active, {rate:5.2f} Hz")


def main() -> None:
    """
    Main execution function demonstrating basic brain usage.
    """
    print("="*60)
    print("NLM BASIC BRAIN EXAMPLE")
    print("="*60)
    print("This example demonstrates the fundamental concepts of NLM:")
    print("1. Creating a brain with configuration")
    print("2. Initializing the brain")
    print("3. Running simulation steps")
    print("4. Observing and reporting brain activity")
    print()
    
    try:
        # Step 1: Create brain
        print("Step 1: Creating brain...")
        brain = create_simple_brain(neuron_count=200)
        print(f"  ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Step 2: Demonstrate basic simulation
        print("\nStep 2: Running basic simulation...")
        stats = simulate_brain(brain, num_steps=50)
        
        # Step 3: Demonstrate activity observation
        print("\nStep 3: Observing neuron activity over time...")
        demonstrate_neuron_activity(brain, steps=30)
        
        # Step 4: Show some useful brain operations
        print("\nStep 4: Additional brain operations...")
        print(f"  ✓ Brain configuration summary:")
        print(f"    {brain.getConfig().summary()}")
        
        # Step 5: Save brain state (demonstrates persistence)
        import os
        import tempfile
        
        print("\nStep 5: Saving brain state...")
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
            brain.save(f.name)
            print(f"  ✓ Saved brain to: {f.name}")
            
            # Load it back
            new_brain = nlm.Brain(brain.getConfig())
            new_brain.load(f.name)
            print(f"  ✓ Successfully loaded brain state")
            
            # Cleanup
            os.unlink(f.name)
        
        print("\n" + "="*60)
        print("EXAMPLE COMPLETED SUCCESSFULLY!")
        print("="*60)
        print("This example demonstrated:")
        print("• Brain creation and initialization")
        print("• Basic simulation stepping")
        print("• Brain activity monitoring")
        print("• Configuration management")
        print("• Brain state persistence")
        
    except Exception as e:
        print(f"\nERROR: {e}")
        print("\nTroubleshooting:")
        print("1. Ensure NLM is properly installed")
        print("2. Check that you have write permissions")
        print("3. Verify system resources are available")
        sys.exit(1)


if __name__ == "__main__":
    main()
