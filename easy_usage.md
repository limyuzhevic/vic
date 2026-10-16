# NLM - Easy Usage Guide for Beginners (Enhanced)

**Version 2.0 - Advanced Usage Included**

## New Features Available

This enhanced version includes **context managers**, **configuration validation**, **error handling**, and **advanced agent features** not available in the basic version.

---

## Advanced Quick Start Examples

### Example 1: Context Manager with Automatic Cleanup

```python
import pynlm

# Using context manager for automatic cleanup
with pynlm.EnhancedConfig() as config:
    config.setValue("brain.neuron_count", 500)
    config.setInt("brain.region_count", 2)
    config.setFloat("learning.rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Config validation: {brain.validateState()}")

# Context manager automatically handles cleanup
print("Context manager completed successfully")
```

### Example 2: Configuration Chaining and Validation

```python
import pynlm

# Create and configure brain using chaining
config = (pynlm.createDefaultConfig()
         .setValue("name", "My Advanced Brain")
         .setInt("neuron_count", 1000)
         .setFloat("simulation_timestep", 0.01)
         .setInt("region_count", 4)
         .setFloat("connection_probability", 0.05))

print(f"Configuration summary:\n{config.summary()}")

# Validate configuration
errors = pynlm.validateConfig(config)
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid!")

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

### Example 3: Complete Agent Workflow with Error Handling

```python
import pynlm

def run_advanced_simulation(steps=200):
    """Run a complete agent simulation with comprehensive error handling"""
    
    # Create configuration with validation
    config = (pynlm.createDefaultConfig()
             .setInt("neuron_count", 200)
             .setInt("region_count", 2)
             .setFloat("simulation_timestep", 0.05))
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with validation
    world = pynlm.createValidatedWorld(30, 30, 10, 10)
    
    # Create and initialize agent
    agent = pynlm.createIntegratedAgent(brain, world)
    
    # Enable all learning features
    agent.enableAllFeatures(True)
    
    print(f"Starting advanced simulation: {steps} steps")
    print(f"Initial brain state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation with comprehensive error handling
    all_errors = []
    for step in range(steps):
        # Safe world update
        world_errors = pynlm.safeWorldUpdate(world, 0.1)
        all_errors.extend(world_errors)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step with error handling
        brain_errors = pynlm.safeBrainStep(brain, step, step * 0.1)
        all_errors.extend(brain_errors)
        
        # Decode and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress periodically
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Final validation
    validation_errors = brain.validateState()
    all_errors.extend(validation_errors)
    
    if all_errors:
        print(f"Simulation completed with {len(all_errors)} warnings/errors")
    else:
        print("Simulation completed successfully!")
    
    return {
        'steps_completed': steps,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'errors': all_errors
    }

# Run the advanced simulation
result = run_advanced_simulation(300)
print(f"\nSimulation Results:")
print(f"  - Steps completed: {result['steps_completed']}")
print(f"  - Final firing neurons: {result['firing_neurons']}")
print(f"  - Total spikes: {result['total_spikes']}")
print(f"  - Development stage: {result['development_stage']}")
print(f"  - Errors encountered: {len(result['errors'])}")
```

### Example 4: Configuration Manager for Preset Environments

```python
import pynlm

# Create a configuration manager
config_manager = pynlm.pybindings.ConfigManager("research_environment")

# Apply preset configurations
print(f"Using configuration: {config_manager.getName()}")

# Try different presets
presets = ["default", "high_performance", "quick_test"]
for preset in presets:
    if config_manager.applyPreset(preset):
        print(f"✓ Applied preset: {preset}")
        
        # Show configuration
        config = config_manager.getConfig()
        print(f"  - Neurons: {config.getOr<int64_t>('neuron_count', 0)}")
        print(f"  - Regions: {config.getOr<int64_t>('region_count', 0)}")
        print(f"  - Connection probability: {config.getOr<float>('connection_probability', 0.0)}")
    else:
        print(f"✗ Failed to apply preset: {preset}")

# Save configuration to file
config_manager.saveToFile("research_config.json")
print("\nConfiguration saved to research_config.json")

# Load configuration from file
new_manager = pynlm.pybindings.ConfigManager("loaded_config")
new_manager.loadFromFile("research_config.json")
print(f"Loaded configuration: {new_manager.getName()}")
```

### Example 5: Batch Operations and Performance Monitoring

```python
import pynlm
import time

def benchmark_config_creation(num_configs=100):
    """Benchmark configuration creation performance"""
    print(f"Benchmarking creation of {num_configs} configurations...")
    
    start_time = time.time()
    configs = []
    
    for i in range(num_configs):
        config = (pynlm.createDefaultConfig()
                 .setInt("neuron_count", 50 + i)
                 .setFloat("simulation_timestep", 0.001 + (i % 10) * 0.001)
                 .setInt("region_count", 1 + (i % 3)))
        configs.append(config)
    
    end_time = time.time()
    
    avg_time = (end_time - start_time) / num_configs
    print(f"  - Average config creation time: {avg_time*1000:.2f} ms")
    print(f"  - Total configurations created: {len(configs)}")
    
    return configs

def batch_brain_creation(configs):
    """Create multiple brains from configurations"""
    print(f"Creating {len(configs)} brains...")
    
    brains = []
    for i, config in enumerate(configs):
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
        
        if i % 20 == 0:
            print(f"  - Created {i+1}/{len(configs)} brains")
    
    print(f"Successfully created {len(brains)} brains")
    return brains

# Run benchmarks
configs = benchmark_config_creation(200)
brains = batch_brain_creation(configs)

# Performance summary
print(f"\nPerformance Summary:")
print(f"  - Configurations created: {len(configs)}")
print(f"  - Brains created: {len(brains)}")
print(f"  - Average neurons per brain: {sum(b->getTotalNeuronCount() for b in brains) / len(brains):.1f}")
```

### Example 6: Debugging and State Management

```python
import pynlm

# Create brain with detailed configuration
config = (pynlm.createDefaultConfig()
         .setInt("neuron_count", 100)
         .setInt("region_count", 2))

brain = pynlm.createBrain(config)
brain.initialize()

print("Initial brain state:")
brain.logStatus()

# Simulate some activity
for step in range(50):
    brain.step(step)
    
    # Check validity periodically
    if step % 10 == 0:
        is_valid = brain.isValid()
        print(f"Step {step}: Brain valid = {is_valid}")
        
        # Get validation errors if any
        if not is_valid:
            errors = brain.validateState()
            print(f"  Validation errors: {errors}")

# Save brain state for debugging
brain.save("debug_checkpoint.bin")
print("\nBrain state saved to debug_checkpoint.bin")

# Load and restore brain state
brain.load("debug_checkpoint.bin")
print("Brain state restored")

# Final status
print("\nFinal brain state:")
brain.logStatus()
```

---

## Quick Reference: New API Methods

### Enhanced Configuration Methods

| Method | Description | Returns |
|--------|-------------|---------|
| `config.setValue(key, value)` | Set config value with chaining | `self` |
| `config.setInt(key, value)` | Set integer value with chaining | `self` |
| `config.setFloat(key, value)` | Set float value with chaining | `self` |
| `config.getValue(key, default)` | Get string value | string |
| `config.getInt(key, default)` | Get integer value | int |
| `config.getFloat(key, default)` | Get float value | float |
| `config.validate()` | Validate configuration | list of errors |
| `config.summary()` | Get configuration summary | string |

### Context Manager Support

```python
# Automatic resource management
with pynlm.EnhancedConfig() as config:
    config.configure(settings)
    # Resources automatically cleaned up
```

### Error Handling Utilities

| Function | Description |
|----------|-------------|
| `pynlm.validateConfig(config)` | Validate configuration |
| `pynlm.validateBrainState(brain)` | Validate brain state |
| `pynlm.safeBrainStep(brain, step, time)` | Safe brain step |
| `pynlm.safeWorldUpdate(world, timestep)` | Safe world update |
| `brain.isValid()` | Check if brain is valid |

### Agent Brain Enhancements

| Method | Description |
|--------|-------------|
| `agent.enableAllFeatures(enable)` | Enable/disable all features |
| `agent.brain` | Property access to underlying brain |

### World Management

| Method | Description |
|--------|-------------|
| `pynlm.createValidatedWorld(width, height, visionWidth, visionHeight)` | Create validated world |
| `world.runEpisode(timestep, steps)` | Run episode with error collection |

---

## Next Steps

When you're comfortable with these advanced features:

1. **Read `HOW_TO_USE.md`** - For detailed technical documentation
2. **Explore `docs/` directory** - For architectural and scientific background
3. **Experiment with different configurations** - Test various preset configurations
4. **Try batch operations** - Process multiple simulations efficiently
5. **Implement your own agents** - Create custom agent behaviors

Happy neural simulation! 🌟
