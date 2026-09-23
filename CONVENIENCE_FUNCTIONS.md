# Convenience Functions Guide

This guide provides comprehensive information about the convenience functions added to the NLM Python bindings to simplify common operations and improve developer productivity.

## Overview

Convenience functions are high-level utilities that provide common patterns and operations that would otherwise require multiple lines of boilerplate code. They are designed to:

- Reduce code duplication
- Improve code readability and maintainability
- Handle common edge cases and error conditions
- Provide sensible defaults for common use cases
- Speed up development by reducing boilerplate

## Factory Functions

### Test Configuration Creation

```python
# Create a standard test configuration
config = pynlm.createTestConfig()

# Returns a Config with typical values:
# - neuron_count: 100
# - region_count: 2
# - connection_probability: 0.1
# - vision_width: 16
# - vision_height: 16
# - learning_rate: 0.01
# - plasticity_enabled: True
# - development_enabled: True
# - curiosity_enabled: True
```

### Test World Creation

```python
# Create a standard test world
world = pynlm.createTestWorld()

# Returns a SimpleWorld with:
# - Dimensions: 100x100
# - Vision: 16x16 pixels
# - Agent start position: (50, 50)
# - Random seed: 42
# - Default energy settings
```

### Test Brain Creation

```python
# Create a test brain with automatic initialization
brain = pynlm.createTestBrain()

# Creates a configuration, brain, and initializes it
# Handles all the boilerplate automatically
```

### Test Agent Creation

```python
# Create a complete test agent with all features enabled
agent = pynlm.createTestAgent()

# Creates:
# - Brain with test configuration
# - World with test settings
# - Agent interface
# - All features enabled (reward, curiosity, development, etc.)
```

## Simulation Convenience Functions

### Complete Simulation

```python
# Run a complete simulation with all boilerplate handled
results = pynlm.runSimulation(brain, world, agent, steps=1000)
spikes, active_neurons, curiosity = results

# This handles:
# 1. World initialization
# 2. Agent setup
# 3. Main simulation loop
# 4. Data collection and return values

print(f"Simulation completed: {spikes} spikes, {active_neurons} active neurons")
```

### Brain Initialization

```python
# Safe brain initialization with error checking
pynlm.initializeBrain(brain)

# This is equivalent to:
# if not brain.initialize():
#     raise NLMError("Failed to initialize brain")
```

### Brain State Management

```python
# Reset brain with safety checks
pynlm.resetBrain(brain)

# This is equivalent to:
# if brain:
#     brain.reset()
# else:
#     raise NLMError("Brain cannot be null")
```

## Validation and Checking Functions

### Configuration Validation

```python
# Validate configuration and get issues
issues = pynlm.validateConfiguration(config)

if issues:
    print(f"Configuration has {len(issues)} issues:")
    for i, issue in enumerate(issues, 1):
        print(f"  {i}. {issue}")
else:
    print("Configuration is valid!")
```

### Brain Statistics

```python
# Get comprehensive brain statistics
stats = pynlm.getBrainStats(brain)
neurons, synapses, active, firing, rate, eireq, spikes, pending = stats

# This returns:
# - neurons: Total neuron count
# - synapses: Total synapse count  
# - active: Active neuron count
# - firing: Firing neuron count
# - rate: Average firing rate
# - eireq: Excitation/inhibition ratio
# - spikes: Total spike count
# - pending: Pending spike event count
```

### Brain Status Printing

```python
# Print detailed brain status and statistics
pynlm.printBrainSummary(brain)

# This is equivalent to:
# if brain:
#     brain.logStatus()
# else:
#     raise NLMError("Brain cannot be null")
```

## Advanced Convenience Patterns

### Batch Operations

```python
# Simulate multiple steps efficiently
def run_simulation_steps(brain, world, agent, num_steps, timestep=0.1):
    """Run multiple simulation steps with proper initialization"""
    # Initialize if needed
    if not brain.getConfig():
        pynlm.initializeBrain(brain)
        world.reset()
        agent.initialize(world)
    
    # Run all steps
    for step in range(num_steps):
        world.update(timestep)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return step

# Usage
steps_completed = run_simulation_steps(brain, world, agent, 1000)
```

### Agent Training Loop

```python
# Common training loop pattern
def train_agent(brain, world, agent, episodes, steps_per_episode):
    """Train an agent with reward-based learning"""
    for episode in range(episodes):
        world.reset()
        agent.initialize(world)
        
        for step in range(steps_per_episode):
            world.update(0.1)
            
            # Get observation
            percept = world.getSensoryPercept()
            
            # Process and act
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # Execute action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward if episode finished
            if step == steps_per_episode - 1:
                agent.applyRewardModulation(result.reward, 0.0)
        
        # Print progress
        if episode % 10 == 0:
            stats = pynlm.getBrainStats(brain)
            print(f"Episode {episode}: {stats[6]} spikes, Curiosity: {agent.getCuriosityLevel():.2f}")
    
    return agent

# Usage
trained_agent = train_agent(brain, world, agent, episodes=100, steps_per_episode=50)
```

### Development Phase Simulation

```python
# Simulate development stages over time
def develop_brain(brain, days=30):
    """Simulate brain development over multiple days"""
    # Convert days to simulation time (assuming 1 day = 10 simulation steps)
    total_steps = days * 10
    
    for step in range(total_steps):
        brain.step(step)
        
        # Check developmental stage changes
        stage = brain.getDevelopmentalStage()
        if stage != DevelopmentalStage::Adult:
            print(f"Step {step}: Current stage = {stage}")
        
        # Log developmental progress
        if step % 100 == 0:
            print(f"Development progress: {step}/{total_steps} steps")
    
    return brain

# Usage
developed_brain = develop_brain(brain, days=30)
```

## Error Handling Convenience

### Safe Function Calls

```python
# Safe wrapper for functions that require non-null inputs
def safe_operation(func, *args, **kwargs):
    """Wrapper that checks for null arguments"""
    if not args or any(arg is None for arg in args if arg is not None):
        raise NLMError("Function requires non-null arguments")
    return func(*args, **kwargs)

# Usage
result = safe_operation(pynlm.runSimulation, brain, world, agent, 100)
```

### Parameter Validation

```python
# Validate common parameters
def validate_simulation_params(steps, timestep):
    """Validate simulation parameters"""
    issues = []
    
    if not isinstance(steps, int) or steps < 1:
        issues.append("Steps must be a positive integer")
    
    if not isinstance(timestep, (int, float)) or timestep <= 0:
        issues.append("Timestep must be a positive number")
    
    if issues:
        raise NLMError(f"Invalid parameters: {', '.join(issues)}")
    
    return True

# Usage
validate_simulation_params(100, 0.1)
```

## Performance Optimization Convenience

### Efficient Statistics Collection

```python
# Collect multiple statistics efficiently
def collect_comprehensive_stats(brain):
    """Collect all available statistics in one call"""
    return {
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'active': brain.getActiveNeuronCount(),
        'firing': brain.getFiringNeuronCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'pending_spikes': brain.getPendingSpikeEventCount(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'config_keys': brain.getConfig().getKeys() if brain.getConfig() else []
    }

# Usage
stats = collect_comprehensive_stats(brain)
print(f"Brain has {stats['neurons']} neurons and {stats['total_spikes']} total spikes")
```

### Memory-Efficient Processing

```python
# Process sensory data without unnecessary copies
def process_sensory_efficiently(agent, world):
    """Process sensory input efficiently"""
    # Get reference to avoid copy
    percept = world.getSensoryPercept()
    
    # Process each sensory modality efficiently
    vision_data = percept.getVision()
    touch_data = percept.getTouch()
    internal_data = percept.getInternal()
    
    # Process each modality
    agent.processSensoryInput(percept)
    
    return {
        'vision_size': len(vision_data),
        'touch_size': len(touch_data),
        'internal_size': len(internal_data),
        'timestamp': percept.getTimestamp()
    }

# Usage
result = process_sensory_efficiently(agent, world)
```

## Best Practices and Migration

### When to Use Convenience Functions

**Use convenience functions when:**
- You need standard boilerplate operations
- You want faster development with standard patterns
- You need error handling for common edge cases
- You're writing test code or educational material
- You want to ensure consistency across your codebase

**Use manual coding when:**
- You need custom behavior not supported by convenience functions
- You're optimizing for performance
- You need fine-grained control over the simulation
- You're writing production code that must be highly optimized
- You need to integrate with external systems

### Migration from Manual to Convenience Code

**Manual Pattern:**
```python
# Manual: 20+ lines of boilerplate
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 2)
config.set("learning_rate", 0.01)

brain = pynlm.createBrain(config)
if not brain.initialize():
    raise Error("Brain initialization failed")

world = pynlm.createSimpleWorld()
world.configure(100, 100, 16, 16)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**Convenience Pattern:**
```python
# Convenience: 2 lines of code
brain = pynlm.createTestBrain()
world = pynlm.createTestWorld()
agent = pynlm.createTestAgent()

# Run simulation (automatically initialized)
pynlm.runSimulation(brain, world, agent, 100)
```

## Troubleshooting Common Issues

### Issue: Convenience functions not working

**Symptoms:** Error messages, unexpected behavior, or no output
**Solutions:**
1. Check function signatures match your needs
2. Verify all required parameters are provided
3. Use try/catch blocks to catch NLMError exceptions
4. Check documentation for examples

### Issue: Performance concerns with convenience functions

**Symptoms:** Slower execution than expected
**Solutions:**
1. Use manual coding for performance-critical sections
2. Batch operations when possible
3. Use the `collect_comprehensive_stats` function for efficiency
4. Profile your code to identify bottlenecks

### Issue: Type mismatches

**Symptoms:** Runtime errors or unexpected behavior
**Solutions:**
1. Use type annotations in your Python code
2. Check documentation for expected parameter types
3. Use the validation functions provided
4. Test with small examples first

## Integration with External Systems

### Machine Learning Integration

```python
# Use convenience functions with scikit-learn
from sklearn.preprocessing import StandardScaler
import numpy as np

# Collect training data
def collect_training_data(brain, world, agent, episodes):
    """Collect training data from simulation"""
    observations = []
    actions = []
    rewards = []
    
    for episode in range(episodes):
        world.reset()
        agent.initialize(world)
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect data
            percept = world.getSensoryPercept()
            observations.append(percept.getAllSignals())
            actions.append(action)
            rewards.append(result.reward)
    
    return np.array(observations), np.array(actions), np.array(rewards)

# Usage
obs, acts, rews = collect_training_data(brain, world, agent, 10)
scaler = StandardScaler()
scaled_obs = scaler.fit_transform(obs)
```

### Visualization Integration

```python
# Use convenience functions with matplotlib
import matplotlib.pyplot as plt

def plot_brain_development(brain, days=30):
    """Plot brain development over time"""
    steps_per_day = 10
    total_steps = days * steps_per_day
    
    spike_counts = []
    firing_rates = []
    
    for day in range(days):
        for step in range(steps_per_day):
            brain.step(day * steps_per_day + step)
        
        # Collect statistics
        stats = pynlm.getBrainStats(brain)
        spike_counts.append(stats[6])  # Total spikes
        firing_rates.append(stats[4])  # Average firing rate
    
    # Create plots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
    
    ax1.plot(spike_counts)
    ax1.set_title("Total Spike Count Over Time")
    ax1.set_xlabel("Days")
    ax1.set_ylabel("Spike Count")
    
    ax2.plot(firing_rates)
    ax2.set_title("Average Firing Rate Over Time")
    ax2.set_xlabel("Days")
    ax2.set_ylabel("Firing Rate (Hz)")
    
    plt.tight_layout()
    plt.show()
```

## Future Enhancements

### Planned Improvements

1. **Additional Convenience Functions**:
   - Multi-agent simulation utilities
   - Advanced visualization helpers
   - Performance profiling tools
   - Validation and testing frameworks

2. **Enhanced Integration**:
   - TensorFlow/PyTorch integration
   - Advanced plotting and visualization
   - Database export/import capabilities
   - Web API integration

3. **Documentation Enhancements**:
   - Interactive tutorials
   - Code examples with documentation
   - API reference with parameters
   - Best practices guide

## Summary

Convenience functions in the NLM Python bindings provide powerful utilities to:

- **Speed up development** with pre-built common patterns
- **Improve reliability** with validated implementations
- **Enhance readability** with clear, self-documenting code
- **Reduce boilerplate** with automated initialization and setup
- **Provide consistency** with standardized patterns across projects

Use convenience functions when you want rapid development and maintainable code, and manual coding when you need maximum control and performance.

The NLM Python bindings now provide both the convenience of high-level utilities and the power of low-level control, giving developers the flexibility to choose the right approach for their specific use case.