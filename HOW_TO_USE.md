# NLM - Complete Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM Python bindings (pynlm). It covers everything from basic usage to advanced configuration and optimization techniques.

The NLM (Neural Learning Machine) is an experimental artificial developmental brain system that implements:
- **LIF neuron dynamics** with real integrate-and-fire computation
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity rules** (STDP, Hebbian, structural)
- **Neuromodulation** (dopamine, curiosity, reward prediction error)
- **Memory systems** (working, episodic, associative)
- **Cognition systems** (planning, concept formation, attention)
- **Developmental processes** (maturation, synaptogenesis, pruning)

## Installation and Setup

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm-project

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build
make -j4

# Install Python bindings
pip install pynlm
```

### Using pip (if available)

```bash
# Install from PyPI (if available)
pip install pynlm
```

## Basic Usage Patterns

### Creating and Running a Simple Brain

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize brain (required before use)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent with World Interaction

```python
import pynlm
import math

def run_simulation(config_file=None, width=20, height=20, num_steps=500):
    # Setup brain
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(width, height, visionWidth=8, visionHeight=8)
    world.setAgentStart(width//2, height//2)
    world.reset()
    
    # Setup agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Main simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Stage: {agent.getDevelopmentalStage().name}")
    
    print("Simulation complete!")
    return brain, world, agent

# Run the simulation
brain, world, agent = run_simulation()
```

## Error Handling and Exception Management

### Basic Error Handling

```python
import pynlm
import traceback

def safe_simulation(config_file=None):
    try:
        config = pynlm.createDefaultConfig()
        
        if config_file:
            if not config.loadFromFile(config_file):
                raise IOError(f"Could not load config file: {config_file}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            
            # Check for errors during simulation
            if brain.getFiringNeuronCount() == 0 and step > 50:
                print(f"Warning: No neurons firing after step {step}")
        
        return brain
        
    except Exception as e:
        print(f"Simulation error: {e}")
        traceback.print_exc()
        return None

# Use safe simulation
brain = safe_simulation("my_config.cfg")
```

### Python-specific Error Handling

```python
def simulate_with_validation(config_file=None):
    config = pynlm.createDefaultConfig()
    
    # Validate configuration before use
    if config_file:
        if not config.loadFromFile(config_file):
            raise ValueError(f"Invalid configuration file: {config_file}")
    
    brain = pynlm.createBrain(config)
    
    # Configure brain with validation
    brain.set("random_seed", 42)  # Ensure reproducibility
    brain.set("neuron_count", 500)
    brain.set("simulation_timestep", 0.001)
    
    try:
        brain.initialize()
        print("Brain initialization successful")
        
        # Verify brain state
        if brain.getTotalNeuronCount() == 0:
            raise RuntimeError("Brain initialized with 0 neurons")
            
    except Exception as e:
        print(f"Brain initialization failed: {e}")
        raise
    
    # Run simulation with error recovery
    for step in range(1000):
        try:
            brain.step(step)
            
            # Check for stability issues
            if brain.getTotalSpikeCount() > 100000:  # Safety threshold
                print(f"Warning: High spike count ({brain.getTotalSpikeCount()}) at step {step}")
                break
                
        except Exception as e:
            print(f"Step {step} failed: {e}")
            # Attempt recovery
            brain.reset()
            brain.initialize()
    
    return brain
```

## Performance Optimization

### Configuration Optimization

```python
def optimized_config():
    """Create an optimized configuration for typical use cases"""
    config = pynlm.createDefaultConfig()
    
    # Core performance settings
    config.set("random_seed", 42)  # For reproducibility
    config.set("simulation_timestep", 0.001)  # Standard timestep
    config.set("neuron_count", 500)  # Smaller for faster testing
    config.set("region_count", 1)
    
    # Plasticity settings
    config.set("stdp_ltp_weight", 0.02f)
    config.set("stdp_ltd_weight", 0.015f)
    config.set("stdp_tau", 20.0f)
    
    # Structural plasticity (usually disabled for performance)
    config.set("synaptogenesis_rate", 0.0001f)
    config.set("pruning_rate", 0.00001f)
    
    # Development (slower but more realistic)
    config.set("development_rate", 0.01f)
    
    return config

# Use optimized configuration
config = optimized_config()
brain = pynlm.createBrain(config)
```

### Simulation Optimization

```python
def optimized_simulation(config, num_steps, log_interval=100):
    """Optimized simulation with performance monitoring"""
    import time
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    start_time = time.time()
    last_log_time = start_time
    
    for step in range(num_steps):
        # Batch world updates if possible
        if step % 10 == 0:
            world.update(0.1)
            
            # Batch sensory processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        # Brain computation (main bottleneck)
        brain.step(step)
        
        # Only decode actions every N steps to reduce overhead
        if step % 5 == 0:
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Performance logging
        current_time = time.time()
        if current_time - last_log_time >= 5.0:  # Every 5 seconds
            steps_per_second = (step + 1) / (current_time - start_time)
            print(f"Step {step}: {steps_per_second:.1f} steps/sec, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
            last_log_time = current_time
    
    end_time = time.time()
    total_time = end_time - start_time
    print(f"Simulation completed in {total_time:.2f} seconds "
          f"({num_steps/total_time:.1f} steps/sec)")
    
    return brain, world, agent
```

### Memory Management

```python
def memory_efficient_simulation(config, max_memory_mb=500):
    """Simulation with memory monitoring"""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    for step in range(1000):
        brain.step(step)
        
        # Monitor memory usage
        current_memory = process.memory_info().rss / 1024 / 1024
        memory_increase = current_memory - initial_memory
        
        if memory_increase > max_memory_mb:
            print(f"Warning: Memory usage ({current_memory:.1f} MB) "
                  f"exceeds limit ({max_memory_mb} MB)")
            # Consider reducing complexity or checkpointing
            break
        
        # Periodic cleanup
        if step % 100 == 0:
            # Clear spike history to save memory
            # This may affect some algorithms, use carefully
            pass
    
    final_memory = process.memory_info().rss / 1024 / 1024
    print(f"Memory usage: {initial_memory:.1f} MB -> {final_memory:.1f} MB "
          f"(increase: {final_memory - initial_memory:.1f} MB)")
    
    return brain
```

## Advanced Features and Configuration

### Exploring Advanced Configuration Options

```python
def explore_configuration_options():
    """Demonstrate advanced configuration options"""
    config = pynlm.createDefaultConfig()
    
    # Get all available configuration keys
    keys = config.getKeys()
    print(f"Available configuration options ({len(keys)}):")
    
    # Show some examples of advanced options
    advanced_examples = [
        ("random_seed", "Random number seed for reproducibility"),
        ("simulation_timestep", "Time step duration in seconds"),
        ("neuron_count", "Total number of neurons in the brain"),
        ("region_count", "Number of neural regions"),
        ("connection_probability", "Probability of synaptic connections"),
        ("stdp_ltp_weight", "STDP long-term potentiation weight"),
        ("stdp_ltd_weight", "STDP long-term depression weight"),
        ("stdp_tau", "STDP time constant in milliseconds"),
        ("synaptogenesis_rate", "Rate of new synapse formation"),
        ("pruning_rate", "Rate of synapse elimination"),
        ("development_rate", "Rate of developmental changes"),
        ("enable_reward_modulation", "Enable reward-based learning"),
        ("enable_curiosity", "Enable curiosity-driven exploration"),
        ("enable_structural_plasticity", "Enable structural learning"),
        ("enable_development", "Enable developmental processes"),
    ]
    
    print("\nAdvanced configuration examples:")
    for key, description in advanced_examples:
        if key in keys:
            value = config.get(key)
            print(f"  {key}: {value} - {description}")
    
    return config

# Explore configuration
config = explore_configuration_options()
```

### Custom World Configuration

```python
def custom_world_simulation(width=30, height=30, vision_width=10, vision_height=10):
    """Create a custom world configuration"""
    world = pynlm.createSimpleWorld()
    
    # Configure world with custom parameters
    world.configure(width, height, vision_width, vision_height)
    
    # Set custom agent start position
    world.setAgentStart(width // 2, height // 2)
    
    # Configure world physics
    world.setMaxEnergy(100.0)  # Maximum agent energy
    world.setEnergyDecayRate(0.01)  # Energy loss per timestep
    world.setRandomSeed(12345)  # For reproducible world generation
    
    # Add custom world objects (e.g., resources, hazards)
    # This requires access to internal world structure
    # world.addObject(world_object)
    
    reset = world.reset()
    print(f"Custom world created: {width}x{height}, vision: {vision_width}x{vision_height}")
    
    return world

# Create custom world
world = custom_world_simulation()
```

## Brain State Management

### Saving and Loading Brain States

```python
def save_load_simulation(config, save_interval=100, max_saves=10):
    """Save and load brain states during simulation"""
    import os
    import shutil
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    save_dir = "brain_checkpoints"
    if not os.path.exists(save_dir):
        os.makedirs(save_dir)
    
    for step in range(1000):
        brain.step(step)
        
        # Save checkpoint periodically
        if step % save_interval == 0 and step > 0:
            # Create save filename
            save_file = os.path.join(save_dir, f"brain_step_{step}.nml")
            
            # Save brain state
            if brain.save(save_file):
                print(f"Saved brain state to: {save_file}")
                
                # Manage save directory size
                saves = [f for f in os.listdir(save_dir) 
                        if f.startswith("brain_step_")]
                saves.sort()
                if len(saves) > max_saves:
                    # Remove oldest saves
                    for old_save in saves[:-max_saves]:
                        os.remove(os.path.join(save_dir, old_save))
            else:
                print(f"Failed to save brain state to: {save_file}")
        
        # Load from specific checkpoint if needed
        # if step == 500:
        #     load_file = os.path.join(save_dir, "brain_step_250.nml")
        #     if brain.load(load_file):
        #         print(f"Loaded brain state from: {load_file}")
    
    return brain
```

## Advanced Agent Configuration

### Configuring Learning Features

```python
def advanced_agent_configuration(brain, world):
    """Configure agent with advanced learning features"""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Configure learning parameters
    agent.set("reward_sensitivity", 1.0)
    agent.set("curiosity_threshold", 0.5)
    agent.set("novelty_detection", True)
    agent.set("prediction_error_weight", 0.1)
    
    # Set developmental stage
    agent.setDevelopmentalStage("CriticalPeriod")
    
    return agent

# Create advanced agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = advanced_agent_configuration(brain, world)
```

### Agent Learning Control

```python
def controlled_learning_simulation(brain, world, agent, phases):
    """Run simulation with controlled learning phases"""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for phase_name, duration, learning_settings in phases:
        print(f"\nStarting phase: {phase_name}")
        
        # Apply phase-specific learning settings
        agent.enableRewardModulation(learning_settings.get("reward", False))
        agent.enableCuriosity(learning_settings.get("curiosity", False))
        agent.enableStructuralPlasticity(learning_settings.get("structural", False))
        agent.enableDevelopment(learning_settings.get("development", False))
        
        # Run phase
        for step in range(duration):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log phase progress
            if step % 50 == 0:
                print(f"  {phase_name} step {step}: "
                      f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return brain, world, agent

# Define learning phases
phases = [
    (" exploration", 200, {"reward": False, "curiosity": True, "structural": False, "development": False}),
    ("reward learning", 300, {"reward": True, "curiosity": True, "structural": False, "development": False}),
    ("structural learning", 250, {"reward": True, "curiosity": True, "structural": True, "development": False}),
    ("development", 250, {"reward": True, "curiosity": True, "structural": True, "development": True}),
]

# Run controlled learning simulation
brain, world, agent = controlled_learning_simulation(
    pynlm.createBrain(pynlm.createDefaultConfig()),
    pynlm.createSimpleWorld(),
    None,  # Will be created inside function
    phases
)
```

## Configuration File Management

### Creating and Managing Configuration Files

```python
def create_configuration_examples():
    """Create example configuration files"""
    config = pynlm.createDefaultConfig()
    
    # Basic configuration
    config.set("random_seed", 42)
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("simulation_timestep", 0.001)
    
    # Save basic configuration
    config.saveToFile("basic_config.cfg")
    print("Created basic_config.cfg")
    
    # Advanced configuration
    config2 = pynlm.createDefaultConfig()
    config2.set("random_seed", 12345)
    config2.set("neuron_count", 500)
    config2.set("region_count", 3)
    config2.set("simulation_timestep", 0.001)
    config2.set("connection_probability", 0.15)
    config2.set("stdp_ltp_weight", 0.02)
    config2.set("stdp_ltd_weight", 0.015)
    config2.set("stdp_tau", 20.0)
    config2.set("synaptogenesis_rate", 0.0001)
    config2.set("pruning_rate", 0.00001)
    config2.set("enable_reward_modulation", True)
    config2.set("enable_curiosity", True)
    config2.set("enable_structural_plasticity", True)
    config2.set("enable_development", True)
    
    # Save advanced configuration
    config2.saveToFile("advanced_config.cfg")
    print("Created advanced_config.cfg")
    
    # Load and verify configuration
    config3 = pynlm.createDefaultConfig()
    if config3.loadFromFile("advanced_config.cfg"):
        print("Successfully loaded advanced_config.cfg")
        print(f"Neuron count: {config3.getOr('neuron_count', 0)}")
        print(f"Reward modulation enabled: {config3.getOr('enable_reward_modulation', False)}")
    
    return config, config2, config3

# Create example configurations
basic_config, advanced_config, loaded_config = create_configuration_examples()
```

## Debugging and Troubleshooting

### Detailed Debugging Information

```python
def debug_simulation(brain, world, agent, config):
    """Run simulation with detailed debugging output"""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("=== NLM Simulation Debug Information ===")
    print(f"Configuration: {config.summary()}")
    print(f"Brain initialization: {brain.initialize()}")
    
    for step in range(100):
        # Get detailed brain state
        print(f"\nStep {step}:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
        print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.4f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.4f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.4f}")
        print(f"  Prediction error: {agent.getPredictionError():.4f}")
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for potential issues
        if brain.getTotalNeuronCount() == 0:
            print("  WARNING: Brain has 0 neurons!")
        
        if brain.getTotalSpikeCount() > 50000:
            print(f"  WARNING: High spike count ({brain.getTotalSpikeCount()}), potential runaway excitation")
        
        if agent.getNeuromodulationLevel() < 0.001:
            print("  WARNING: Very low neuromodulation level")
        
        if agent.getCuriosityLevel() < 0.001:
            print("  WARNING: Very low curiosity level")
    
    print("\n=== Simulation Complete ===")
    return brain, world, agent
```

### Performance Analysis

```python
def performance_analysis(brain, world, agent, num_steps=500):
    """Analyze simulation performance and identify bottlenecks"""
    import time
    import statistics
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("=== Performance Analysis ===")
    
    # Time different operations
    step_times = []
    firing_rates = []
    spike_counts = []
    neuromodulation_levels = []
    
    start_time = time.time()
    
    for step in range(num_steps):
        step_start = time.time()
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step_time = time.time() - step_start
        step_times.append(step_time)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
        neuromodulation_levels.append(agent.getNeuromodulationLevel())
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"Total simulation time: {total_time:.2f} seconds")
    print(f"Average step time: {statistics.mean(step_times)*1000:.2f} ms")
    print(f"Min step time: {min(step_times)*1000:.2f} ms")
    print(f"Max step time: {max(step_times)*1000:.2f} ms")
    print(f"Step time std dev: {statistics.stdev(step_times)*1000:.2f} ms")
    print(f"Steps per second: {num_steps/total_time:.1f}")
    
    print(f"\nFiring rate statistics:")
    print(f"  Mean: {statistics.mean(firing_rates):.4f} Hz")
    print(f"  Min: {min(firing_rates):.4f} Hz")
    print(f"  Max: {max(firing_rates):.4f} Hz")
    print(f"  Std dev: {statistics.stdev(firing_rates):.4f} Hz")
    
    print(f"\nSpike count statistics:")
    print(f"  Final: {spike_counts[-1]}")
    print(f"  Mean per step: {statistics.mean(spike_counts):.1f}")
    print(f"  Max per step: {max(spike_counts)}")
    
    print(f"\nNeuromodulation level statistics:")
    print(f"  Mean: {statistics.mean(neuromodulation_levels):.4f}")
    print(f"  Min: {min(neuromodulation_levels):.4f}")
    print(f"  Max: {max(neuromodulation_levels):.4f}")
    
    # Identify potential bottlenecks
    slow_steps = [(i, t) for i, t in enumerate(step_times) if t > statistics.mean(step_times) * 2]
    if slow_steps:
        print(f"\nSlow steps identified:")
        for step_idx, step_time in slow_steps[:5]:  # Show first 5
            print(f"  Step {step_idx}: {step_time*1000:.2f} ms")
    
    # Analyze firing rate stability
    if statistics.stdev(firing_rates) / statistics.mean(firing_rates) > 0.5:
        print("\nWARNING: High firing rate variability (>50% of mean)")
    
    # Analyze neuromodulation patterns
    avg_neuromodulation = statistics.mean(neuromodulation_levels)
    if avg_neuromodulation < 0.01:
        print("\nWARNING: Low average neuromodulation level (< 0.01)")
    elif avg_neuromodulation > 0.5:
        print("\nWARNING: High average neuromodulation level (> 0.5)")
    
    return {
        'total_time': total_time,
        'steps_per_second': num_steps/total_time,
        'avg_step_time': statistics.mean(step_times),
        'firing_rate_mean': statistics.mean(firing_rates),
        'firing_rate_std': statistics.stdev(firing_rates),
        'final_spike_count': spike_counts[-1],
        'neuromodulation_mean': avg_neuromodulation
    }
```

## Best Practices and Recommendations

### Coding Best Practices

```python
def best_practices_example():
    """Demonstrate best practices for NLM programming"""
    
    # 1. Always initialize configuration explicitly
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 42)  # For reproducibility
    
    # 2. Validate configuration before use
    if not config.has("neuron_count"):
        config.set("neuron_count", 100)  # Set sensible defaults
    
    # 3. Create brain with validated configuration
    brain = pynlm.createBrain(config)
    
    # 4. Initialize brain with error checking
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # 5. Use descriptive variable names
    simulation_world = pynlm.createSimpleWorld()
    simulation_world.configure(20, 20, 8, 8)
    
    # 6. Document simulation parameters
    simulation_steps = 1000
    time_step = 0.1
    
    # 7. Log simulation progress
    for step in range(simulation_steps):
        simulation_world.update(time_step)
        
        # Log at intervals
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
    
    # 8. Save final results
    brain.save("simulation_results.nml")
    
    return brain

# Use best practices
brain = best_practices_example()
```

### Common Pitfalls and How to Avoid Them

```python
def avoid_common_pitfalls():
    """Demonstrate how to avoid common NLM programming pitfalls"""
    
    print("=== Common Pitfalls and Solutions ===\n")
    
    # Pitfall 1: Forgetting to initialize the brain
    print("1. Forgetting to initialize the brain:")
    print("   ❌ brain = pynlm.createBrain(config)  # Forgot .initialize()")
    print("   ✅ brain = pynlm.createBrain(config)")
    print("       brain.initialize()  # Always initialize!\n")
    
    # Pitfall 2: Using default values without validation
    print("2. Using default values without validation:")
    print("   ❌ config = pynlm.createDefaultConfig()")
    print("       # ... run simulation ...")
    print("   ✅ config = pynlm.createDefaultConfig()")
    print("       if not config.has('neuron_count'):")
    print("           config.set('neuron_count', 100)")
    print("       if not config.has('simulation_timestep'):")
    print("           config.set('simulation_timestep', 0.001)\n")
    
    # Pitfall 3: Not checking simulation results
    print("3. Not checking simulation results:")
    print("   ❌ brain.step(step)  # No error checking")
    print("   ✅ try:")
    print("       brain.step(step)")
    print("       if brain.getFiringNeuronCount() == 0 and step > 50:")
    print("           print('Warning: No neurons firing')\n")
    
    # Pitfall 4: Memory leaks from not saving results
    print("4. Memory leaks from not saving results:")
    print("   ❌ # Run simulation, but never save results")
    print("   ✅ brain.save('simulation_results.nml')  # Save intermediate results\n")
    
    # Pitfall 5: Poor reproducibility
    print("5. Poor reproducibility:")
    print("   ❌ config = pynlm.createDefaultConfig()")
    print("   ✅ config = pynlm.createDefaultConfig()")
    print("       config.set('random_seed', 12345)  # For reproducibility\n")
    
    # Pitfall 6: Ignoring error handling
    print("6. Ignoring error handling:")
    print("   ❌ world.update(0.1)  # No error checking")
    print("   ✅ try:")
    print("       world.update(0.1)")
    print("       if world.getWidth() == 0:")
    print("           raise ValueError('World configuration invalid')\n")
    
    # Pitfall 7: Inefficient simulations
    print("7. Inefficient simulations:")
    print("   ❌ for i in range(100000):")
    print("       brain.step(i)  # No logging, no progress checks")
    print("   ✅ for i in range(100000):")
    print("       brain.step(i)")
    print("       if i % 1000 == 0:")
    print("           print(f'Step {i}/{100000}')\n")
    
    print("All pitfalls have good solutions. Use these best practices for robust simulations!")

# Follow best practices
avoid_common_pitfalls()
```

## Next Steps and Further Learning

### Continuing Your NLM Journey

Once you're comfortable with the basics, consider these next steps:

1. **Read the Architecture Documentation**
   - `docs/ARCHITECTURE.md` explains the complete NLM system design
   - `docs/SCIENCE.md` covers the scientific background
   - `docs/ROADMAP.md` outlines future development plans

2. **Explore Advanced Configurations**
   - Experiment with different neural parameters
   - Try different plasticity rules and neuromodulators
   - Configure complex world environments

3. **Study Performance Optimization**
   - Profile your simulations to identify bottlenecks
   - Optimize configuration for your specific use case
   - Consider parallel processing for large-scale simulations

4. **Contribute to the Project**
   - Report issues and suggest improvements
   - Share your configurations and simulation results
   - Help improve the documentation with your examples

### Getting Help

If you need help with NLM:

1. **Documentation**: Check this file and the existing `easy_usage.md`
2. **GitHub Issues**: Report bugs and request features
3. **Community**: Join the NLM discussion forums
4. **Examples**: Study the provided example configurations

### Reference Material

- **Python API Reference**: Generated documentation from bindings.cpp
- **C++ API Documentation**: Header files in `src/` directory
- **Configuration Schema**: Configuration options and descriptions
- **Example Configurations**: Various `.cfg` files in the repository

---

*Document generated from NLM source code analysis.*
*NLM version: 0.1.0 - Phase 6: Final Integration*
*Last updated: $(date)*