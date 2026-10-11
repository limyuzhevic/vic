# Tutorial 1: Simple Brain Creation and Simulation

This tutorial provides a comprehensive introduction to creating and running your first neural simulation with NLM. It covers the basics of brain initialization, simulation execution, and observing neural activity.

## Learning Objectives

After completing this tutorial, you should be able to:

1. **Create a basic brain configuration** using NLM's configuration system
2. **Initialize and run a neural simulation** with LIF neurons
3. **Observe neural activity** through spike counts and firing rates
4. **Save and load brain states** for persistence
5. **Monitor simulation progress** with basic statistics

## What You'll Learn

- Brain anatomy: neurons, synapses, regions, and connectivity
- Neural dynamics: how LIF neurons process information
- Simulation workflow: from initialization to termination
- Data collection: extracting meaningful statistics from neural activity
- State management: saving and restoring brain states

## Prerequisites

- NLM framework installed (see Getting Started Guides)
- Basic understanding of neural networks and terminology
- Comfort with Python programming (C++ examples also available)

## Step 1: Creating Your First Brain

### Understanding Brain Configuration

A NLM brain is created from a configuration that specifies:

- **Number of neurons**: Total neural population size
- **Connectivity**: How neurons connect to each other
- **Neuron parameters**: Threshold, resting potential, time constants
- **Plasticity settings**: Learning rules and parameters

### Creating a Simple Configuration

Let's start with the simplest possible configuration:

```python
import pynlm

# Create default configuration (1000 neurons, standard settings)
config = pynlm.createDefaultConfig()
print(f"Default config has {config.get('brain.neuron_count')} neurons")

# Optional: Customize configuration
config.set("brain.neuron_count", 100)  # Reduce for faster testing
config.set("plasticity.stdp.enable", True)  # Enable learning
config.set("plasticity.hebbian.enable", True)  # Add Hebbian learning
```

### The Default Configuration Explained

The `createDefaultConfig()` function creates a configuration with sensible defaults:

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `brain.neuron_count` | 1000 | Total neurons in the network |
| `brain.synapse_density` | 0.1 | About 10% of possible connections |
| `brain.connection_probability` | 0.05 | Each neuron connects to ~5% others |
| `brain.v_thresh` | -50.0 | Neuron fires when potential > -50mV |
| `brain.v_rest` | -70.0 | Resting potential at -70mV |
| `brain.v_reset` | -75.0 | After firing, resets to -75mV |
| `plasticity.stdp.enable` | true | Spike-timing dependent plasticity |
| `plasticity.hebbian.enable` | true | Hebbian learning (fire together, wire together) |

## Step 2: Brain Initialization

### What Happens During Initialization

When you call `brain.initialize()`, the brain performs several setup operations:

1. **Allocates memory** for neurons and synapses
2. **Connects neurons** according to configuration
3. **Initializes synaptic weights** with random values
4. **Sets up event systems** for spike propagation
5. **Prepares plasticity mechanisms** for learning

### Your First Brain Creation Code

```python
import pynlm
import time

def create_first_brain():
    """Create and return a basic brain for simulation"""
    
    # Step 1: Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)  # Small brain for quick testing
    
    # Step 2: Create brain
    brain = pynlm.createBrain(config)
    
    # Step 3: Initialize brain
    print("Initializing brain...")
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    print(f"   ✓ Brain initialized in {init_time:.3f} seconds")
    print(f"   ✓ Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   ✓ Total synapses: {brain.getTotalSynapseCount()}")
    
    return brain

def explore_brain_structure(brain):
    """Explore and display brain structure information"""
    
    print("\nBrain Structure Analysis:")
    print("=" * 40)
    
    # Get information about different neuron types
    for neuron_type in pynlm.NeuronType:
        count = brain.getNeuronCount(neuron_type)
        if count > 0:
            print(f"   {neuron_type.name:15}: {count} neurons")
    
    # Get regional information
    print(f"\n   Brain regions: {brain.getRegionCount()} regions")
    for region_id in range(1, brain.getRegionCount() + 1):
        region = brain.getRegion(region_id)
        if region:
            neurons = region.getNeuronCount()
            synapses = region.getSynapseCount()
            print(f"   Region {region_id:2}: {neurons:4d} neurons, {synapses:4d} synapses")

# Example usage
if __name__ == "__main__":
    # Create your first brain
    brain = create_first_brain()
    
    # Explore its structure
    explore_brain_structure(brain)
    
    # Clean up
    brain.reset()
```

### Expected Output

```
Initializing brain...
   ✓ Brain initialized in 0.234 seconds
   ✓ Total neurons: 100
   ✓ Total synapses: 1,234

Brain Structure Analysis:
========================================
    EXCITATORY       : 80 neurons
    INHIBITORY        : 20 neurons
    SENSORY           : 0 neurons
    MOTOR             : 0 neurons
    MODULATORY        : 0 neurons
    INTERNAL          : 0 neurons

   Brain regions: 1
   Region  1:   100 neurons, 1,234 synapses
```

## Step 3: Running a Neural Simulation

### Understanding the Simulation Loop

The core of NLM is the simulation loop, where:

1. **Time advances** in small steps (typically 0.001 seconds)
2. **Neurons update** their membrane potentials
3. **Spikes are detected** when thresholds are crossed
4. **Spikes propagate** through synaptic connections
5. **Plasticity rules** modify synaptic weights
6. **Statistics are collected** for analysis

### Basic Simulation Loop

```python
import pynlm

def run_basic_simulation(brain, num_steps=100):
    """Run a basic neural simulation"""
    
    print(f"\nRunning simulation for {num_steps} steps...")
    print("Step | Firing | Spikes | Rate (Hz) | Activity")
    print("-----|--------|--------|-----------|---------")
    
    # Initial statistics
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run simulation
    for step in range(num_steps):
        # Execute one simulation step
        brain.step(step)
        
        # Collect statistics every 10 steps
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount() - initial_spikes
            rate = (total_spikes / (step + 1)) * 1000  # Convert to Hz
            
            # Determine activity level
            if firing == 0:
                activity = "Quiet"
            elif firing < 10:
                activity = "Low"
            elif firing < 30:
                activity = "Moderate"
            elif firing < 60:
                activity = "High"
            else:
                activity = "Very High"
            
            print(f"{step:4d} | {firing:6d} | {total_spikes:6d} | "
                  f"{rate:8.1f} | {activity:8s}")
    
    # Final statistics
    final_spikes = brain.getTotalSpikeCount() - initial_spikes
    average_rate = (final_spikes / num_steps) * 1000
    
    print(f"\nSimulation complete!")
    print(f"   Total steps: {num_steps}")
    print(f"   Total spikes: {final_spikes}")
    print(f"   Average firing rate: {average_rate:.2f} Hz")
    print(f"   Average firing neurons: {brain.getAverageFiringRate()}")

def run_enhanced_simulation(brain, num_steps=500, inject_current=False):
    """Run an enhanced simulation with optional input and monitoring"""
    
    print(f"\nRunning enhanced simulation for {num_steps} steps...")
    
    # Enable different plasticity mechanisms
    agent = pynlm.createAgentBrain(brain)
    
    # Optionally enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        # Optional: Inject current to stimulate neurons
        if inject_current and step % 50 == 0:
            # Inject current into sensory neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 20.0)
        
        # Run brain step
        brain.step(step)
        
        # Optional: Get action (even without world for testing)
        if step % 100 == 0:
            action = agent.decodeMotorCommand()
            if action != pynlm.ActionType.None:
                print(f"   Step {step}: Decoded action {action}")

def demonstrate_neural_dynamics(brain, num_steps=200):
    """Demonstrate various neural dynamics"""
    
    print(f"\nDemonstrating neural dynamics for {num_steps} steps...")
    
    # Track various statistics
    spike_history = []
    firing_history = []
    time_points = []
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 5 == 0:  # Sample every 5 steps
            spike_history.append(brain.getTotalSpikeCount())
            firing_history.append(brain.getFiringNeuronCount())
            time_points.append(step)
            
            # Print every 50 steps
            if step % 50 == 0:
                print(f"   Step {step:4d}: {brain.getFiringNeuronCount():3d} neurons firing, "
                      f"{brain.getTotalSpikeCount():6d} total spikes")
    
    # Analyze results
    if len(spike_history) > 10:
        print(f"\nNeural Dynamics Analysis:")
        print(f"   Spike rate trend: {'Increasing' if spike_history[-1] > spike_history[0] else 'Decreasing'}")
        print(f"   Max firing rate: {max(firing_history)}")
        print(f"   Average firing rate: {sum(firing_history)/len(firing_history):.1f}")
        
        # Check for learning
        if len(spike_history) >= 20:
            first_half_avg = sum(spike_history[:10]) / 10
            second_half_avg = sum(spike_history[-10:]) / 10
            if second_half_avg > first_half_avg * 1.5:
                print(f"   ✓ Learning detected: Spike activity increased by {((second_half_avg/first_half_avg - 1)*100):.1f}%")
            else:
                print(f"   ! No clear learning trend detected")
```

### Complete Simulation Example

```python
import pynlm
import time

def run_complete_simulation():
    """Complete example of creating and running a neural simulation"""
    
    print("=== NLM Neural Simulation Tutorial ===")
    print("This example demonstrates creating a brain, running a simulation, and analyzing results.\n")
    
    # 1. Create and configure brain
    print("1. Creating brain configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)  # Small network for demonstration
    config.set("plasticity.stdp.enable", True)  # Enable STDP learning
    config.set("plasticity.hebbian.enable", True)  # Enable Hebbian learning
    
    # 2. Create and initialize brain
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Network size: {brain.getTotalNeuronCount()} neurons")
    print(f"   Connectivity: {brain.getTotalSynapseCount()} synapses")
    
    # 3. Run different simulation phases
    print("\n3. Running simulation phases...")
    
    # Phase 1: Baseline simulation (no external input)
    print("   Phase 1: Baseline simulation...")
    run_basic_simulation(brain, 100)
    
    # Reset for next phase
    brain.reset()
    brain.initialize()
    
    # Phase 2: With stimulation
    print("   Phase 2: With external stimulation...")
    run_enhanced_simulation(brain, 200, inject_current=True)
    
    # 4. Save brain state
    print("\n4. Saving brain state...")
    brain.save("tutorial_brain_state.bin")
    print("   ✓ Brain state saved to tutorial_brain_state.bin")
    
    # 5. Demonstrate neural dynamics
    print("\n5. Analyzing neural dynamics...")
    demonstrate_neural_dynamics(brain, 150)
    
    print("\n=== Tutorial Complete ===")
    print("Your first NLM brain simulation has been successfully run!")
    print("Check tutorial_brain_state.bin for the saved brain state.")

if __name__ == "__main__":
    run_complete_simulation()
```

## Step 4: Observing and Analyzing Neural Activity

### Monitoring Neural Activity

Understanding how to monitor neural activity is crucial for working with NLM:

```python
import pynlm

def monitor_neural_activity(brain, duration_steps=300):
    """Monitor and display neural activity over time"""
    
    print(f"\nMonitoring neural activity for {duration_steps} steps...")
    print("Step | Firing | Rate (Hz) | % Active | Stability")
    print("-----|--------|-----------|----------|----------")
    
    # Collect baseline statistics
    baseline_firing = brain.getFiringNeuronCount()
    baseline_total = brain.getTotalSpikeCount()
    
    for step in range(duration_steps):
        brain.step(step)
        
        # Calculate statistics
        firing = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount() - baseline_total
        rate = (total_spikes / (step + 1)) * 1000
        percent_active = (firing / brain.getTotalNeuronCount()) * 100
        
        # Assess stability (coefficient of variation of firing)
        if step >= 50:
            recent_steps = min(20, step)
            # This would require more complex tracking for true stability measure
            stability = "Stable" if abs(firing - baseline_firing) < 10 else "Variable"
        else:
            stability = "Warming up"
        
        # Print every 50 steps
        if step % 50 == 0:
            print(f"{step:4d} | {firing:6d} | {rate:9.1f} | "
                  f"{percent_active:8.1f}% | {stability:8s}")
    
    print(f"\nActivity Analysis:")
    print(f"   Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Max firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes accumulated: {brain.getTotalSpikeCount() - baseline_total}")

# Example usage with activity monitoring
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    monitor_neural_activity(brain, 200)
    
    brain.reset()
```

### Understanding Key Metrics

| Metric | What it Means | Typical Values |
|--------|---------------|----------------|
| **Firing Neurons** | Number of neurons currently active | 0-100 (out of total) |
| **Firing Rate (Hz)** | Average spikes per second | 0-100 Hz typical |
| **% Active** | Percentage of neurons firing | 0-100% |
| **Stability** | How consistent firing is over time | Variable vs. steady |

### Visualizing Neural Activity

For more detailed analysis, you can create visualizations:

```python
import pynlm
import matplotlib.pyplot as plt

def create_neural_activity_plots(brain, num_steps=500):
    """Create plots showing neural activity over time"""
    
    # Collect data
    times = list(range(num_steps))
    firing_rates = []
    spike_counts = []
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 10 == 0:  # Sample every 10 steps
            firing_rates.append(brain.getFiringNeuronCount())
            spike_counts.append(brain.getTotalSpikeCount())
    
    # Create plots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
    
    # Plot 1: Firing rate over time
    ax1.plot(times[::10], firing_rates)
    ax1.set_xlabel('Time Step')
    ax1.set_ylabel('Firing Neurons')
    ax1.set_title('Neural Activity Over Time')
    ax1.grid(True, alpha=0.3)
    
    # Plot 2: Spike count accumulation
    ax2.plot(times[::10], [s - spike_counts[0] for s in spike_counts])
    ax2.set_xlabel('Time Step')
    ax2.set_ylabel('Cumulative Spikes')
    ax2.set_title('Spike Accumulation Over Time')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('neural_activity.png', dpi=150, bbox_inches='tight')
    plt.show()
    
    print("Activity plots saved to 'neural_activity.png'")

# Usage
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    create_neural_activity_plots(brain, 200)
    
    brain.reset()
```

## Step 5: Saving and Loading Brain States

### State Management

NLM provides powerful state management for:

1. **Saving simulation results** for later analysis
2. **Resuming simulations** from checkpoints
3. **Comparing brain states** across different runs
4. **Exporting data** for other tools

### Basic State Management

```python
import pynlm

def save_and_load_example():
    """Demonstrate saving and loading brain states"""
    
    print("=== Brain State Management Example ===\n")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("1. Initial brain state:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Synapses: {brain.getTotalSynapseCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    # Run some simulation
    print("\n2. Running simulation...")
    for step in range(50):
        brain.step(step)
    
    print(f"   After simulation:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Synapses: {brain.getTotalSynapseCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save brain state
    print("\n3. Saving brain state...")
    brain.save("example_brain.bin")
    print("   ✓ Brain state saved to 'example_brain.bin'")
    
    # Reset brain and reload
    print("\n4. Resetting and reloading...")
    brain.reset()
    brain.initialize()
    
    print("   After reset:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Synapses: {brain.getTotalSynapseCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    # Load saved state
    brain.load("example_brain.bin")
    
    print("   After loading saved state:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Synapses: {brain.getTotalSynapseCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\n=== State Management Complete ===")

if __name__ == "__main__":
    save_and_load_example()
```

### Advanced State Features

```python
import pynlm

def advanced_state_features():
    """Demonstrate advanced brain state features"""
    
    # Create configuration with development
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("development.stage_interval", 200)  # Development every 200 steps
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable development tracking
    agent = pynlm.createAgentBrain(brain)
    agent.enableDevelopment(True)
    
    print("=== Advanced State Management ===\n")
    
    # Monitor development over time
    print("Monitoring brain development over 500 steps:")
    print("Step | Development Stage | Firing | Learning Rate")
    print("-----|------------------|--------|--------------")
    
    for step in range(500):
        brain.step(step)
        agent.updateDevelopment(0.1)  # Update development each step
        
        if step % 50 == 0:
            dev_stage = brain.getDevelopmentalStage()
            firing = brain.getFiringNeuronCount()
            # Estimate learning rate from spike activity
            learning_rate = min(1.0, firing / 50.0)
            
            print(f"{step:4d} | {str(dev_stage):17} | {firing:6d} | {learning_rate:12.2f}")
    
    # Save detailed state with development info
    print("\nSaving detailed brain state...")
    brain.save("advanced_brain_state.bin")
    
    # Analyze saved state
    print("\nAnalyzing saved state...")
    brain.reset()
    brain.initialize()
    brain.load("advanced_brain_state.bin")
    
    print(f"Reloaded brain statistics:")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Current firing rate: {brain.getAverageFiringRate():.2f} Hz")

if __name__ == "__main__":
    advanced_state_features()
```

## Step 6: Extending Your First Brain

### Adding Learning Mechanisms

Now that you have a working brain, let's add learning capabilities:

```python
import pynlm

def create_learning_brain():
    """Create a brain with learning capabilities"""
    
    # Configuration with all learning mechanisms
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Learning Brain Configuration:")
    print("   ✓ STDP plasticity enabled")
    print("   ✓ Hebbian learning enabled")
    print("   ✓ Structural plasticity enabled")
    print("   ✓ Reward modulation enabled")
    print("   ✓ Curiosity module enabled")
    print("   ✓ Development tracking enabled")
    
    return brain, agent

def run_learning_simulation(brain, agent, num_steps=300):
    """Run a simulation with learning enabled"""
    
    print(f"\nRunning learning simulation for {num_steps} steps...")
    
    # Track learning progress
    learning_metrics = {
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'spike_history': []
    }
    
    for step in range(num_steps):
        # Simulate some environmental input
        if step % 100 == 0:
            # Present "stimulus" to trigger learning
            curiosity = agent.getCuriosityLevel()
            print(f"   Step {step}: Curiosity level = {curiosity:.3f}")
        
        # Run brain step
        brain.step(step)
        
        # Apply neuromodulation
        reward = 0.1 if step % 50 == 0 else 0.0  # Occasional reward
        predicted = 0.0
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if step % 50 == 0:
            learning_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            learning_metrics['novelty_levels'].append(agent.getNoveltyLevel())
            learning_metrics['prediction_errors'].append(agent.getPredictionError())
            learning_metrics['spike_history'].append(brain.getTotalSpikeCount())
    
    # Analyze learning
    print(f"\nLearning Analysis:")
    if learning_metrics['curiosity_levels']:
        initial_curiosity = learning_metrics['curiosity_levels'][0]
        final_curiosity = learning_metrics['curiosity_levels'][-1]
        curiosity_change = final_curiosity - initial_curiosity
        
        print(f"   Curiosity change: {curiosity_change:+.3f}")
        if curiosity_change > 0.1:
            print("   ✓ Brain is getting more curious (exploration increasing)")
        elif curiosity_change < -0.1:
            print("   ! Brain is becoming less curious (familiarity increasing)")
        else:
            print("   = Curiosity level stable")
    
    return learning_metrics

# Complete learning example
if __name__ == "__main__":
    # Create learning brain
    brain, agent = create_learning_brain()
    
    # Run learning simulation
    metrics = run_learning_simulation(brain, agent, 300)
    
    # Save learned brain
    brain.save("learning_brain.bin")
    print(f"\n✓ Learned brain saved to 'learning_brain.bin'")
    
    brain.reset()
```

## Complete Tutorial Summary

### What You've Accomplished

1. ✅ **Created a basic brain configuration** using NLM's settings system
2. ✅ **Initialized and ran a neural simulation** with LIF neurons
3. ✅ **Observed neural activity** through spike counts and firing rates
4. ✅ **Saved and loaded brain states** for persistence
5. ✅ **Monitored simulation progress** with basic statistics
6. ✅ **Extended your brain** with learning mechanisms

### Key Takeaways

- **Brains need initialization**: Always call `brain.initialize()` before use
- **Simulation is incremental**: Each `brain.step()` advances time by one step
- **Statistics are cumulative**: Spikes and firing rates accumulate over time
- **State persistence**: Save checkpoints for resuming simulations
- **Learning is optional**: Enable neuromodulation and plasticity for learning

### Next Steps

Now that you have a working brain, you can:

1. **Create complete agent-world loops** using Tutorial 2
2. **Explore complex learning** using Tutorial 3
3. **Study memory systems** using Tutorial 4
4. **Understand motivation** using Tutorial 5
5. **Study brain development** using Tutorial 6

### Further Reading

- [Python API Documentation](./docs/PYTHON_API.md)
- [Architecture Overview](./docs/ARCHITECTURE.md)
- [Phase 6 Integration](./docs/PHASE6_ARCHITECTURE.md)
- [Configuration Reference](./docs/CONFIGURATION.md)

This tutorial provides a solid foundation for working with NLM. The neural systems you've created are ready for more complex interactions and learning experiences in subsequent tutorials.
