#!/usr/bin/env python3
"""
NLM Example 2: Brain-Step Operations

This example focuses on understanding how brain.step() works and what
information you can gather about neural activity and state.

Learning Objectives:
- Master brain.step() method usage
- Understand neural state information
- Learn how to monitor brain development
- Practice data collection and analysis

Time Complexity: O(n_steps × n_neurons) for simulation

Expected Output (simplified):
```
=== Brain-Step Operations Example ===
Brain initialized with 1000 neurons
Step 0: Firing=12, Rate=1.2%, TotalSpikes=15
Step 1: Firing=8, Rate=0.8%, TotalSpikes=28
...
Development stage: CriticalPeriod (stage 1)
Average firing rate: 1.5 Hz
Motor commands available: Yes
=== Example Complete ===
```

Tips for Customization:
- Add sensory input to increase firing rates
- Enable neuromodulation for learning
- Increase simulation time for development
- Monitor specific neuron types

Key Concepts Demonstrated:
1. Step-by-step neural processing
2. State monitoring and statistics
3. Development progression
4. Activity-dependent changes
"""

import pynlm
import time
import statistics

def main():
    print("=== Brain-Step Operations Example ===")
    print("This example demonstrates the core NLM operation: brain.step()")
    print("We'll collect detailed statistics about neural activity and")
    print("observe how the brain develops over time.\n")
    
    # Step 1: Setup
    print("Step 1: Setting up brain and monitoring...")
    config = pynlm.createDefaultConfig()
    
    # Enable features for more interesting behavior
    config.set("brain.neuron_count", 200)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons")
    print(f"Plasticity enabled: STDP={brain.getPlasticityStatus('STDP')}")
    
    # Step 2: Data collection
    print("\nStep 2: Running simulation with data collection...")
    
    # Collect statistics
    firing_rates = []
    spike_counts = []
    development_stages = []
    active_neurons_per_step = []
    
    for step in range(50):
        # Record state before step
        before_spikes = brain.getTotalSpikeCount()
        before_firing = brain.getFiringNeuronCount()
        
        # Run the brain step
        brain.step(step)
        
        # Record state after step
        after_spikes = brain.getTotalSpikeCount()
        after_firing = brain.getFiringNeuronCount()
        new_spikes = after_spikes - before_spikes
        firing_rate = brain.getAverageFiringRate()
        dev_stage = brain.getDevelopmentalStage()
        
        # Store statistics
        firing_rates.append(firing_rate)
        spike_counts.append(new_spikes)
        development_stages.append(dev_stage)
        active_neurons_per_step.append(after_firing)
        
        # Log periodically
        if step % 10 == 0:
            print(f"Step {step}: Firing={after_firing}, Rate={firing_rate:.2f}Hz, "
                  f"NewSpikes={new_spikes}, Stage={dev_stage.name}")
    
    # Step 3: Analysis
    print("\nStep 3: Analyzing collected data...")
    
    # Calculate statistics
    avg_firing_rate = statistics.mean(firing_rates) if firing_rates else 0
    max_spikes_step = max(spike_counts) if spike_counts else 0
    max_spike_step = spike_counts.index(max_spikes_step) if spike_counts else -1
    
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Maximum spikes in one step: {max_spikes_step} (at step {max_spike_step})")
    print(f"Development stage at end: {development_stages[-1].name}")
    print(f"Average active neurons: {statistics.mean(active_neurons_per_step):.1f}")
    
    # Step 4: Brain state information
    print("\nStep 4: Additional brain state information...")
    
    # Check different neuron types
    print("\nNeuron type distribution:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Currently firing: {brain.getFiringNeuronCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Development stage details
    print(f"\nDevelopment details:")
    print(f"  Current stage: {brain.getDevelopmentalStage().name}")
    print(f"  Stage number: {brain.getDevelopmentalStage().value}")
    print(f"  Simulation time: {brain.getSimulationTime():.3f}")
    
    # Plasticity status
    print(f"\nPlasticity systems active:")
    print(f"  STDP: {brain.getPlasticityStatus('STDP')}")
    print(f"  Hebbian: {brain.getPlasticityStatus('Hebbian')}")
    print(f"  Structural: {brain.getPlasticityStatus('Structural')}")
    
    # Step 5: Future exploration
    print("\n=== Understanding the Results ===")
    print("Key observations:")
    print("1. Firing rate varies over time - brain is dynamic")
    print("2. Development stage progresses with simulation time")
    print("3. Some spikes occur, showing neural processing")
    print("4. Plasticity systems are ready for learning")
    
    print("\nNext steps for exploration:")
    print("- Add sensory input to increase firing")
    print("- Create a world to provide stimuli")
    print("- Enable neuromodulation for learning")
    print("- Run longer simulations for development")
    
    print("\n=== Example Complete ===")
    print(f"Collected {len(firing_rates)} data points")
    print("This example demonstrates how to monitor and analyze brain activity.")

if __name__ == "__main__":
    start_time = time.time()
    main()
    end_time = time.time()
    print(f"\nExecution time: {end_time - start_time:.2f} seconds")
