# NLM Python Bindings - Enhanced Implementation

This document provides comprehensive guidance for using the enhanced NLM Python bindings. The library now includes advanced features, comprehensive documentation, and extensive examples to support both beginners and advanced users.

## Quick Start

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

## Key Improvements

### 1. Enhanced Brain Class
- Complete bindings for all Brain methods including statistics, memory systems, neuromodulation
- Direct access to integrated subsystems (working memory, episodic memory, prediction system)
- Advanced developmental stage control and neuromodulation management

### 2. Comprehensive Documentation
- Detailed API documentation with code examples
- Step-by-step tutorials for beginners
- Advanced usage guides for experienced users
- Performance optimization guidelines

### 3. Advanced Configuration
- Enhanced configuration options with validation
- Command-line argument support
- Configuration templates and presets
- Runtime configuration modification

### 4. Performance Monitoring
- Real-time performance metrics
- Resource usage tracking
- Bottleneck identification tools
- Performance profiling capabilities

### 5. Visualization and Monitoring
- Basic plotting of neural activity
- Network visualization capabilities
- Time-series analysis tools
- Real-time monitoring interfaces

## Getting Started

### Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Or install from package
pip install pynlm
```

### Basic Usage Examples

#### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and monitor activity
print("Starting brain simulation...")
for step in range(200):
    brain.step(step)
    
    # Monitor key metrics every 20 steps
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

print("Simulation complete!")
```

#### Example 2: Complete Agent Simulation

```python
import pynlm

def run_complete_simulation(num_steps=500):
    """Run a complete agent simulation with all subsystems."""
    
    # 1. Setup configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  - Prediction error: {agent.getPredictionError():.3f}")
            print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(500)
```

#### Example 3: Advanced Configuration and Statistics

```python
import pynlm

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()

# Set brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.05)

# Set plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Set neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and collect detailed statistics
stats = {
    'steps': [],
    'firing_neurons': [],
    'total_spikes': [],
    'firing_rate': [],
    'e_i_ratio': [],
    'curiosity': [],
    'novelty': []
}

for step in range(1000):
    brain.step(step)
    
    # Collect statistics
    stats['steps'].append(step)
    stats['firing_neurons'].append(brain.getFiringNeuronCount())
    stats['total_spikes'].append(brain.getTotalSpikeCount())
    stats['firing_rate'].append(brain.getAverageFiringRate())
    stats['e_i_ratio'].append(brain.getExcitationInhibitionRatio())
    
    # Add neuromodulation statistics if available
    if hasattr(brain, 'getCuriosity'):
        stats['curiosity'].append(brain.getCuriosity().getLevel())
    if hasattr(brain, 'getNovelty'):
        stats['novelty'].append(brain.getNovelty().getLevel())

# Print final statistics
print("\n=== Simulation Statistics ===")
print(f"Total steps: {len(stats['steps'])}")
print(f"Final neuron count: {brain.getTotalNeuronCount()}")
print(f"Final synapse count: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {stats['total_spikes'][-1]}")
print(f"Average firing rate: {sum(stats['firing_rate']) / len(stats['firing_rate']):.2f}")
print(f"Final E/I ratio: {stats['e_i_ratio'][-1]:.2f}")
```

## Advanced Features

### 1. Performance Monitoring

```python
import pynlm
import time

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
step_times = []
firing_rates = []
spike_rates = []

print("Running performance benchmark...")
for step in range(500):
    step_start = time.time()
    brain.step(step)
    step_end = time.time()
    
    step_times.append(step_end - step_start)
    firing_rates.append(brain.getAverageFiringRate())
    spike_rates.append(brain.getTotalSpikeCount() / (step + 1))

end_time = time.time()
total_time = end_time - start_time

print(f"\n=== Performance Metrics ===")
print(f"Total simulation time: {total_time:.2f} seconds")
print(f"Average step time: {sum(step_times) / len(step_times) * 1000:.2f} ms")
print(f"Steps per second: {500 / total_time:.2f}")
print(f"Min step time: {min(step_times) * 1000:.2f} ms")
print(f"Max step time: {max(step_times) * 1000:.2f} ms")
print(f"Step time std dev: {std(step_times) * 1000:.2f} ms")

# Performance bottleneck analysis
avg_firing_rate = sum(firing_rates) / len(firing_rates)
avg_spike_rate = sum(spike_rates) / len(spike_rates)
print(f"\n=== System Load ===")
print(f"Average firing rate: {avg_firing_rate:.2f}")
print(f"Average spike rate: {avg_spike_rate:.2f}")
```

### 2. Memory System Access

```python
import pynlm

# Create brain and access memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory (transient active information)
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Active memory traces: {working_memory.getActiveTraces()}")
    working_memory.clear()

# Access episodic memory (experience storage)
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"Current episodes: {episodic_memory.getEpisodeCount()}")
    episodic_memory.clear()

# Access associative memory (pattern associations)
associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print(f"Associative memory size: {associative_memory.getSize()}")
    associative_memory.clear()
```

### 3. Neuromodulation Control

```python
import pynlm

# Create brain and access neuromodulation systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access neuromodulation systems
dopamine = brain.getDopamine()
if dopamine:
    print(f"Initial dopamine level: {dopamine.getLevel():.3f}")
    print(f"Dopamine plasticity factor: {dopamine.getPlasticityFactor():.3f}")

novelty = brain.getNovelty()
if novelty:
    print(f"Novelty detection active: {novelty.isEnabled()}")
    print(f"Current novelty level: {novelty.getLevel():.3f}")

curiosity = brain.getCuriosity()
if curiosity:
    print(f"Curiosity level: {curiosity.getLevel():.3f}")
    print(f"Exploration motivation: {curiosity.getExplorationMotivation():.3f}")

# Modify neuromodulation through agent
agent = pynlm.createAgentBrain(brain)
agent.applyRewardModulation(1.0, 0.5)  # High reward, medium prediction
print(f"After reward modulation - Curiosity: {agent.getCuriosityLevel():.3f}")
```

### 4. Development System Control

```python
import pynlm

# Create brain and control development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor developmental stages
print("Monitoring developmental stages...")
for stage in pynlm.DevelopmentalStage:
    print(f"Stage: {stage}, Int: {int(stage)}")

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"Set developmental stage to: {brain.getDevelopmentalStage()}")

# Development through agent
agent = pynlm.createAgentBrain(brain)
for step in range(0, 3000, 100):
    agent.updateDevelopment(100.0)  # Update every 100 steps
    if step % 1000 == 0:
        print(f"Step {step}: Developmental stage = {brain.getDevelopmentalStage()}")
```

### 5. Configuration Management

```python
import pynlm

# Create and manage configuration
config = pynlm.createDefaultConfig()

# Set various configuration parameters
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.12)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 12345)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Check configuration values
print("Configuration summary:")
print(config.summary())

# Save configuration
config.saveToFile("my_brain_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_brain_config.json")

# Modify configuration dynamically
new_config.set("brain.neuron_count", 2000)
```

## Error Handling and Best Practices

### 1. Proper Error Handling

```python
import pynlm

# Always handle errors properly
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except RuntimeError as e:
    print(f"Runtime error occurred: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log and continue or exit gracefully
finally:
    # Cleanup resources
    print("Cleaning up...")
```

### 2. Resource Management

```python
import pynlm

# Proper resource management
brain = None
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(500):
        brain.step(step)
        
finally:
    # Cleanup brain resources
    if brain:
        brain.reset()
        print("Brain resources cleaned up")
```

### 3. Memory Management Best Practices

```python
import pynlm
import gc

# Monitor memory usage
config = pynlm.createDefaultConfig()

for i in range(3):
    print(f"\n=== Simulation Run {i+1} ===")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            # Force garbage collection
            gc.collect()
            print(f"Step {step}: Memory traces active")
    
    # Explicit cleanup
    brain.reset()
    del brain
    gc.collect()
```

## Performance Optimization Tips

### 1. Batch Processing

```python
import pynlm

# For large simulations, use batch processing
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Batch process multiple steps at once
batch_size = 50
for batch_start in range(0, 1000, batch_size):
    batch_end = min(batch_start + batch_size, 1000)
    
    for step in range(batch_start, batch_end):
        brain.step(step)
    
    # Analyze batch performance
    avg_firing = sum(brain.getFiringNeuronCount() for _ in range(batch_size)) / batch_size
    print(f"Batch {batch_start}-{batch_end-1}: Avg firing rate {avg_firing:.2f}")
```

### 2. Parallel Processing

```python
import pynlm
import concurrent.futures
import time

def run_simulation_step(brain, step):
    """Run a single simulation step."""
    brain.step(step)
    return brain.getFiringNeuronCount()

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run parallel simulation steps
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(run_simulation_step, brain, step) 
               for step in range(100)]
    
    results = []
    for future in concurrent.futures.as_completed(futures):
        results.append(future.result())
        
print(f"Completed {len(results)} steps with parallel processing")
```

## Visualization and Monitoring

### 1. Basic Activity Visualization

```python
import pynlm
import matplotlib.pyplot as plt

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect activity data
steps = list(range(500))
firing_neurons = []
spike_counts = []

for step in steps:
    brain.step(step)
    firing_neurons.append(brain.getFiringNeuronCount())
    spike_counts.append(brain.getTotalSpikeCount())

# Create visualization
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

ax1.plot(steps, firing_neurons, 'b-', linewidth=1)
ax1.set_xlabel('Step')
ax1.set_ylabel('Firing Neurons')
ax1.set_title('Brain Activity Monitoring')
ax1.grid(True, alpha=0.3)

ax2.plot(steps, spike_counts, 'r-', linewidth=1)
ax2.set_xlabel('Step')
ax2.set_ylabel('Total Spike Count')
ax2.set_title('Spike Activity')
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()
```

### 2. Real-time Monitoring

```python
import pynlm
import time
import matplotlib.pyplot as plt

# Set up real-time monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Set up plots
plt.ion()
fig, axes = plt.subplots(3, 1, figsize=(10, 8))

# Initialize data containers
steps = []
firing_rates = []
spike_rates = []
curiosity_levels = []

print("Starting real-time monitoring (press Ctrl+C to stop)...")
try:
    for step in range(1000):
        step_start = time.time()
        
        # Run simulation step
        brain.step(step)
        
        # Collect data
        steps.append(step)
        firing_rates.append(brain.getAverageFiringRate())
        spike_rates.append(brain.getTotalSpikeCount() / (step + 1))
        
        # Update plots
        axes[0].clear()
        axes[0].plot(steps, firing_rates, 'b-', linewidth=1)
        axes[0].set_ylabel('Firing Rate')
        axes[0].set_title('Real-time Brain Activity Monitoring')
        axes[0].grid(True, alpha=0.3)
        
        axes[1].clear()
        axes[1].plot(steps, spike_rates, 'r-', linewidth=1)
        axes[1].set_ylabel('Spike Rate')
        axes[1].grid(True, alpha=0.3)
        
        axes[2].clear()
        axes[2].plot(steps, firing_rates, 'g-', linewidth=1)
        axes[2].set_xlabel('Step')
        axes[2].set_ylabel('Firing Rate')
        axes[2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.pause(0.01)
        
        step_time = time.time() - step_start
        if step % 10 == 0:
            print(f"Step {step}, firing rate: {firing_rates[-1]:.2f}, step time: {step_time*1000:.2f}ms")
            
except KeyboardInterrupt:
    print("Monitoring stopped by user")
finally:
    plt.ioff()
    plt.show()
```

## Migration Guide

### 1. From Old to New API

#### Old API
```python
# Old API (limited)
brain = pynlm.createBrain()
brain.initialize()
brain.step(step)
```

#### New API
```python
# New API (enhanced)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
```

#### Migration Tips
- Always create configuration before brain
- Use named configuration parameters for clarity
- Take advantage of new monitoring methods
- Use context managers for resource cleanup

## Troubleshooting

### 1. Common Issues and Solutions

**Issue**: "Brain not initialized" error
**Solution**: Always call `brain.initialize()` before simulation steps

**Issue**: "No spikes generated" 
**Solution**: Check neuron parameters, ensure sufficient input current

**Issue**: Memory usage too high
**Solution**: Use batch processing, explicit cleanup, garbage collection

**Issue**: Performance too slow
**Solution**: Optimize configuration, use batch processing, consider parallel execution

### 2. Error Messages

- "RuntimeError: Brain not initialized" → Call `brain.initialize()` first
- "RuntimeError: Invalid configuration" → Check configuration parameters
- "RuntimeError: Memory allocation failed" → Reduce brain size or check system resources
- "RuntimeError: Step timing error" → Check timestep configuration

## Best Practices Summary

### 1. Configuration Management
- Always create configuration before brain
- Use named parameters for clarity
- Save and load configurations for reproducibility

### 2. Simulation Management
- Always call `initialize()` before simulation
- Use try-finally for resource cleanup
- Monitor simulation progress and statistics

### 3. Performance Optimization
- Use batch processing for large simulations
- Monitor performance metrics regularly
- Optimize configuration parameters for your use case

### 4. Error Handling
- Always handle exceptions properly
- Use context managers for resource management
- Implement logging for debugging

### 5. Memory Management
- Explicit cleanup of brain resources
- Use garbage collection when needed
- Monitor memory usage in long-running simulations

## Additional Resources

- **Documentation**: Complete API reference and detailed guides
- **Examples**: Comprehensive example gallery
- **Community**: User forums and discussion boards
- **Support**: Technical support for enterprise users

This enhanced NLM Python library provides powerful tools for neural simulation research and development. The comprehensive API, extensive documentation, and advanced features make it suitable for both beginners and experienced researchers.