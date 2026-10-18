# NLM - Advanced Python Usage Guide

This document covers advanced usage patterns, best practices, and newly added convenience functions for the NLM Python bindings.

## New Convenience Functions

### Quick Test and Validation

```python
import pynlm

# Quick test to verify bindings work
pynlm.quick_test()

# Create and run a simple brain, returning the object
brain = pynlm.create_and_run_brain(neuronCount=500, numSteps=1000)

# Get formatted brain statistics
summary = pynlm.get_brain_summary(brain)
print(summary)

# Get detailed statistics
detailed_stats = pynlm.get_detailed_stats(brain)
```

### World Configuration Helpers

```python
import pynlm

# Create world with convenience function
world = pynlm.create_minimal_world(width=20, height=20)

# Configure world using helper
pynlm.configure_world(world, width=30, height=30, visionWidth=12, visionHeight=12)

# Set agent position
pynlm.set_agent_start_position(world, 15.0, 15.0)

# Add resources
pynlm.add_resource_to_world(world, 5.0, 5.0, 2.0)
pynlm.add_resource_to_world(world, 10.0, 10.0, 1.0)
```

### Complete Agent Setup

```python
import pynlm

# Create complete agent setup in one call
brain, world, agent = pynlm.create_complete_agent(
    neuronCount=1000, 
    worldSize=25
)

# The setup enables all subsystems by default
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Optionally disable some features if needed
agent.enableCuriosity(False)  # Save computation cycles
agent.enableStructuralPlasticity(False)
```

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

# Validate required keys
required_keys = ["brain.neuron_count", "brain.synapse_density"]
try:
    pynlm.validate_config(config, required_keys)
    print("Configuration is valid!")
except Exception as e:
    print(f"Configuration error: {e}")

# Safe value access with defaults
neuron_count = pynlm.safe_get_int(config, "brain.neuron_count", 100)
synapse_density = pynlm.safe_get_double(config, "brain.synapse_density", 0.05)
reward_mod = pynlm.safe_get_bool(config, "neuromod.dopamine.enabled", True)
```

### Performance Benchmarking

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark performance
benchmark_results = pynlm.benchmark_brain_operations(brain, steps=10000, warmupSteps=1000)
print(benchmark_results)
```

### Complete Agent Simulation Pattern

```python
import pynlm

def run_advanced_agent_simulation(steps=1000):
    """Advanced agent simulation with all features enabled."""
    
    # Create complete agent setup
    brain, world, agent = pynlm.create_complete_agent(
        neuronCount=800,
        worldSize=20
    )
    
    # Customize configuration
    brain.getConfig().set("brain.v_thresh", -45.0)
    brain.getConfig().set("brain.tau_ref", 5.0)
    brain.getConfig().set("plasticity.stdp.learning_rate", 0.01)
    
    # Initialize agent
    agent.initialize(world)
    
    # Optionally disable expensive features
    agent.enableStructuralPlasticity(True)  # Keep for development learning
    agent.enableCuriosity(True)              # Keep for exploration
    agent.enableRewardModulation(True)      # Keep for reinforcement learning
    agent.enableDevelopment(True)           # Keep for maturation
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if we have reward signal
        internal_signals = percept.getInternal()
        if internal_signals:
            reward = internal_signals[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress occasionally
        if step % 100 == 0:
            stats = pynlm.get_detailed_stats(brain)
            print(f"\n=== Step {step} ===")
            print(stats)
            print(f"Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"Developmental Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_advanced_agent_simulation(1000)
```

## Best Practices

### 1. Configuration Management

```python
# Create config with specific settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -50.0)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### 2. Error Handling

```python
import pynlm

# Use try-catch for robust applications
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -10)  # Invalid value
    print("Config created")
except Exception as e:
    print(f"Error creating config: {e}")

try:
    brain = pynlm.create_and_run_brain(neuronCount=100, numSteps=100)
    print("Brain created successfully")
except Exception as e:
    print(f"Error: {e}")
```

### 3. State Management

```python
import pynlm

# Create and save brain state
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save state for later
brain.save("my_brain_checkpoint.bin")

# Later, load the saved state
config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)
brain2.initialize()
brain2.load("my_brain_checkpoint.bin")

# The loaded brain continues from where it left off
for i in range(100, 200):
    brain2.step(i)
```

### 4. Memory Usage Considerations

```python
import pynlm

# Create brain with memory usage in mind
brain = pynlm.create_simple_brain()

# Monitor memory (placeholder)
memory_info = pynlm.get_memory_usage()
print(memory_info)

# If memory usage is high, consider:
# - Reducing neuron count
# - Disabling structural plasticity
# - Using smaller world dimensions
```

### 5. Performance Optimization

```python
import pynlm

# Create brain for performance testing
brain = pynlm.create_simple_brain()

# Run benchmark
benchmark_results = pynlm.benchmark_brain_operations(brain, steps=50000, warmupSteps=1000)
print(benchmark_results)

# Use benchmark results to:
# - Optimize neuron count for performance
# - Disable expensive features
# - Tune simulation parameters
```

## Migration from Basic to Advanced Usage

### Before (Basic Usage)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

### After (Advanced Usage)
```python
import pynlm

# Create complete setup with all features
brain, world, agent = pynlm.create_complete_agent(
    neuronCount=500,
    worldSize=10
)

# Initialize agent
agent.initialize(world)

# Run sophisticated simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## API Reference for New Functions

### Core Convenience Functions
- `pynlm.__version__`: Python module version string
- `pynlm.__all__`: List of exported symbols

### Configuration Helpers
- `pynlm.validate_config(config, required_keys)`: Validates required config keys
- `pynlm.safe_get_int(config, key, default)`: Safe integer retrieval
- `pynlm.safe_get_double(config, key, default)`: Safe double retrieval
- `pynlm.safe_get_string(config, key, default)`: Safe string retrieval
- `pynlm.safe_get_bool(config, key, default)`: Safe boolean retrieval

### World Helpers
- `pynlm.create_minimal_world(width, height)`: Creates world with default settings
- `pynlm.configure_world(world, width, height, visionWidth, visionHeight)`: Configures world
- `pynlm.set_agent_start_position(world, x, y)`: Sets agent starting position
- `pynlm.add_resource_to_world(world, x, y, value)`: Adds resource to world

### Brain Helpers
- `pynlm.quick_test()`: Quick verification of bindings
- `pynlm.create_and_run_brain(neuronCount, numSteps)`: Create and run brain
- `pynlm.get_brain_summary(brain)`: Get brain summary as string
- `pynlm.get_detailed_stats(brain)`: Get detailed brain statistics

### Agent Helpers
- `pynlm.create_simple_brain()`: Create simple brain with default config
- `pynlm.create_complete_agent(neuronCount, worldSize)`: Create complete agent setup
- `pynlm.run_simulation(brain, world, agent, numSteps)`: Run simulation

### Performance and Validation
- `pynlm.get_memory_usage()`: Get memory usage information
- `pynlm.benchmark_brain_operations(brain, steps, warmupSteps)`: Benchmark brain operations

## Examples of Advanced Usage

### 1. Development Stage Monitoring

```python
import pynlm

brain, world, agent = pynlm.create_complete_agent(neuronCount=200)

for step in range(500):
    brain.step(step)
    
    # Monitor development
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage}")
    
    # Development progresses through stages automatically
    # Initial → CriticalPeriod → Maturation → Adult → Aging
```

### 2. Curiosity-Driven Exploration

```python
import pynlm

brain, world, agent = pynlm.create_complete_agent(neuronCount=100)

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check curiosity level
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.8:
        # High curiosity suggests exploration behavior
        print(f"High curiosity at step {step}: {curiosity:.3f}")
    
    # Action will naturally incorporate curiosity
    action = agent.decodeMotorCommand()
```

### 3. Structured Experimentation

```python
import pynlm

# Create different experiment configurations
experiments = {
    "basic": lambda: pynlm.create_simple_brain(),
    "world": lambda: pynlm.create_minimal_world(10, 10),
    "complete": lambda: pynlm.create_complete_agent(100, 10),
}

def run_experiment(name, steps=100):
    """Run experiment with given configuration."""
    create_func = experiments[name]
    
    if name == "complete":
        brain, world, agent = create_func()
        agent.initialize(world)
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            # ... simulation loop
            
    elif name == "basic":
        brain = create_func()
        for step in range(steps):
            brain.step(step)
    
    elif name == "world":
        world = create_func()
        for step in range(steps):
            world.update(0.1)
    
    return "Experiment completed"
```

## Troubleshooting

### Common Issues

1. **Segmentation Faults**
   ```python
   # Always initialize before use
   brain = pynlm.createBrain(config)
   brain.initialize()  # Forgets this at your peril!
   ```

2. **Memory Issues**
   ```python
   # Monitor memory usage
   memory_info = pynlm.get_memory_usage()
   print(memory_info)
   ```

3. **Performance Problems**
   ```python
   # Use benchmark to identify bottlenecks
   benchmark_results = pynlm.benchmark_brain_operations(brain, steps=1000)
   print(benchmark_results)
   ```

4. **Configuration Problems**
   ```python
   # Validate configuration
   pynlm.validate_config(config, ["brain.neuron_count"])
   ```

## Future Enhancements

This document and the Python bindings will continue to evolve with:

1. More sophisticated convenience functions
2. Advanced visualization capabilities
3. Enhanced error recovery mechanisms
4. Performance profiling tools
5. Distributed computing support
6. Model export/import utilities
7. Integration with scientific computing libraries