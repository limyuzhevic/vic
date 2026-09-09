# Enhanced NLM Python API Examples

This document provides comprehensive examples for using the enhanced NLM Python API with improved usability, method chaining, batch operations, and better error handling.

## Getting Started

First, make sure you have the underlying pynlm module installed:

```bash
# Install with scikit-build-core (recommended for development)
pip install scikit-build-core pybind11
pip install -e .

# Or from source
mkdir -p build
cd build
cmake ..
make -j4
pip install scikit-build-core pybind11
```

Once installed, you can use the enhanced API by importing from the `nlm_enhanced_api` module or by importing `pynlm` and using the new convenience methods.

## Basic Usage

### 1. Simple Brain Simulation

```python
import nlm_enhanced_api as nlm

# Create and initialize brain with method chaining
config = nlm.NLMConfig().set_neuron_count(2000).set_learning_rate(0.001)
brain = nlm.NLMBrain(config).initialize()

# Run simulation steps
brain.step(100).step(50)  # Chain multiple steps

print(f"Created brain with {brain.get_total_neuron_count()} neurons")
print(f"Total spikes: {brain.get_total_spike_count()}")
```

### 2. Brain in a World

```python
import nlm_enhanced_api as nlm

# Create world
world = nlm.NLMWorld().configure(50, 50, 8, 8)

# Create brain with preset configuration
brain = nlm.NLMBrain(
    nlm.create_enhanced_config(
        neuron_count=1500,
        synapse_density=0.1,
        learning_rate=0.001
    )
).initialize()

# Create agent brain interface
agent = nlm.NLMAgentBrain(brain).initialize(world)

# Enable learning systems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Run simulation with periodic statistics
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Apply reward modulation
    reward = percept.get_internal()[0] if percept.get_internal() else 0.0
    agent.apply_reward(reward)
    
    # Update development
    agent.update_development(0.1)
    
    if step % 20 == 0:
        stats = brain.get_statistics()
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")
```

## Advanced Usage

### 3. Batch Operations for Performance

```python
import nlm_enhanced_api as nlm

# Create brain and configure in batch
config = nlm.NLMConfig().batch_set({
    'brain.neuron_count': 5000,
    'brain.synapse_density': 0.2,
    'plasticity.stdp.learning_rate': 0.002,
    'plasticity.hebbian.enable': True,
    'plasticity.structural.enable': True,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True
})

brain = nlm.NLMBrain(config).initialize()

# Run batch steps with time stamps
steps_with_time = [(i, i * 0.1) for i in range(100)]
brain.step_with_time(steps_with_time)

# Batch current injection
brain.inject_current_batch(nlm.pynlm.NeuronType.Excitatory, 1.0)
```

### 4. Enhanced Error Handling

```python
import nlm_enhanced_api as nlm

# Safe operation with error handling
def safe_brain_operation(brain, operation_name, operation):
    return nlm.safe_operation(
        operation, 
        default=None, 
        operation_name=operation_name
    )

# Example usage
stats = safe_brain_operation(
    brain, 
    "get_statistics",
    brain.get_statistics
)

if stats:
    print(f"Brain stats: {stats}")
else:
    print("Failed to get statistics")

# Try-catch for specific error types
try:
    config = nlm.NLMConfig().load_from_file("config.json")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

### 5. Complete Simulation Episode

```python
import nlm_enhanced_api as nlm

def run_enhanced_episode(num_steps=500, timestep=0.1):
    """Run a complete enhanced simulation episode."""
    
    # Setup with method chaining
    config = (nlm.NLMConfig()
              .set_neuron_count(2000)
              .set_synapse_density(0.1)
              .set_learning_rate(0.001))
    
    brain = nlm.NLMBrain(config).initialize()
    world = nlm.NLMWorld().configure(30, 30, 6, 6)
    agent = nlm.NLMAgentBrain(brain).initialize(world)
    
    # Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run episode
    results = nlm.run_simulation_episode(
        brain, world, agent, num_steps, timestep
    )
    
    # Analyze results
    print(f"Episode completed: {results['steps_completed']} steps")
    print(f"Total actions: {len(results['actions_taken'])}")
    print(f"Total rewards: {sum(results['episode_rewards']):.2f}")
    
    final_stats = results['final_brain_state']
    print(f"Final stats - Firing neurons: {final_stats['firing_neurons']}")
    print(f"Final stats - Average firing rate: {final_stats['average_firing_rate']:.2f}")
    
    return results

# Run the episode
episode_results = run_enhanced_episode(1000)
```

### 6. Statistics and Analytics

```python
import nlm_enhanced_api as nlm
import pandas as pd

# Create brain
brain = nlm.NLMBrain(nlm.NLMConfig()).initialize()

# Get comprehensive statistics
stats = brain.get_statistics()
print("=== Brain Statistics ===")
for key, value in stats.items():
    print(f"{key}: {value}")

# Compare statistics over time
print("\n=== Statistics Analysis ===")
print(f"E/I Ratio: {stats['excitation_inhibition_ratio']:.3f}")
print(f"Firing Ratio: {stats['firing_neurons']/max(stats['total_neurons'], 1):.3f}")
print(f"Activity Level: {stats['average_firing_rate']:.2f} Hz")
```

### 7. Region Management

```python
import nlm_enhanced_api as nlm

# Create brain
brain = nlm.NLMBrain(nlm.NLMConfig()).initialize()

# Create regions with descriptive names
region1 = brain.add_region("sensory_cortex")
region2 = brain.add_region("motor_cortex")
region3 = brain.add_region("associative_cortex")

print(f"Created regions: {brain.get_region_count()}")
print(f"Region IDs: {brain.get_region_ids()}")

# Access regions
for region_id in brain.get_region_ids():
    region = brain.get_region(region_id)
    if region:
        print(f"Region {region_id}: Active")
    else:
        print(f"Region {region_id}: Not found")
```

### 8. State Management and Checkpoints

```python
import nlm_enhanced_api as nlm
import tempfile
import os

# Create brain
brain = nlm.NLMBrain(nlm.NLMConfig()).initialize()

# Run some simulation steps
brain.step(100)

# Save state with automatic file creation
with tempfile.TemporaryDirectory() as temp_dir:
    checkpoint_file = os.path.join(temp_dir, "brain_checkpoint.bin")
    
    # Save checkpoint
    brain.save(checkpoint_file)
    print(f"Brain state saved to {checkpoint_file}")
    
    # Create a new brain and load the checkpoint
    new_brain = nlm.NLMBrain(nlm.NLMConfig())
    new_brain.load(checkpoint_file)
    
    # Verify the load worked
    print(f"Loaded brain has {new_brain.get_total_neuron_count()} neurons")
    print(f"Original brain had {brain.get_total_neuron_count()} neurons")
    
    # Both should have the same number of neurons
    assert brain.get_total_neuron_count() == new_brain.get_total_neuron_count()
```

## Best Practices

### 1. Use Method Chaining for Fluency

```python
# Good: Fluent interface
brain = (nlm.NLMBrain(
    nlm.NLMConfig()
      .set_neuron_count(2000)
      .set_learning_rate(0.001)
)
.initialize()
.step(50)
.step(100)
)

# Bad: Multiple separate statements
config = nlm.NLMConfig()
config.set_neuron_count(2000)
config.set_learning_rate(0.001)
brain = nlm.NLMBrain(config)
brain.initialize()
brain.step(50)
brain.step(100)
```

### 2. Validate Configuration Before Use

```python
import nlm_enhanced_api as nlm

config = (nlm.NLMConfig()
          .set_neuron_count(1000)
          .set_learning_rate(0.01))

# Validate required keys
config.validate_required_keys(['brain.neuron_count', 'brain.learning_rate'])

# Create brain with validated config
brain = nlm.NLMBrain(config).initialize()
```

### 3. Handle Errors Gracefully

```python
import nlm_enhanced_api as nlm

def create_and_run_brain(config_dict=None):
    try:
        # Create configuration
        if config_dict:
            config = nlm.NLMConfig().batch_set(config_dict)
        else:
            config = nlm.NLMConfig()
        
        # Validate configuration
        config.validate_required_keys(['brain.neuron_count'])
        
        # Create and initialize brain
        brain = nlm.NLMBrain(config).initialize()
        
        # Run simulation
        brain.step(100)
        
        # Get results
        stats = brain.get_statistics()
        return {"success": True, "stats": stats}
        
    except nlm.ValidationError as e:
        return {"success": False, "error": f"Validation error: {e}"}
    except nlm.ConfigurationError as e:
        return {"success": False, "error": f"Configuration error: {e}"}
    except nlm.MemoryError as e:
        return {"success": False, "error": f"Memory error: {e}"}
    except nlm.NLMError as e:
        return {"success": False, "error": f"NLM error: {e}"}
    except Exception as e:
        return {"success": False, "error": f"Unexpected error: {e}"}
```

## Performance Tips

### 1. Use Batch Operations

```python
import nlm_enhanced_api as nlm

# Process multiple sensory inputs at once
inputs = [sensory1, sensory2, sensory3, sensory4, sensory5]
brain.receive_sensory_input_batch(inputs)

# Update world with multiple timesteps
timesteps = [0.1, 0.2, 0.3, 0.4, 0.5]
world.update_batch(timesteps)

# Get batch of statistics
stats_batch = []
for step in range(100):
    brain.step(step)
    stats_batch.append(brain.get_statistics())
```

### 2. Pre-allocate Resources

```python
import nlm_enhanced_api as nlm

# Pre-allocate with appropriate sizes
config = nlm.create_enhanced_config(
    neuron_count=10000,  # Increase for larger simulations
    synapse_density=0.2,
    learning_rate=0.001
)

# Pre-allocate world
world = nlm.NLMWorld().configure(100, 100, 20, 20)
world.set_max_energy(1000.0)  # Higher energy for longer simulations
```

## Migration Guide

### From Basic NLM to Enhanced API

If you're currently using the basic NLM API and want to upgrade to the enhanced version:

```python
# OLD WAY (basic NLM)
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(100)
action = brain.produceAction()

# NEW WAY (enhanced NLM)
import nlm_enhanced_api as nlm

config = nlm.NLMConfig().load_from_file("config.json")
brain = nlm.NLMBrain(config).initialize()
brain.step(100)
action = brain.produce_action()
```

### Key Changes:

1. **Configuration**: `createDefaultConfig()` → `NLMConfig()`
2. **Brain creation**: `createBrain(config)` → `NLMBrain(config)`
3. **Initialization**: `brain.initialize()` → `.initialize()` (chainable)
4. **Steps**: `brain.step(n)` → `.step(n)` (chainable)
5. **Actions**: `brain.produceAction()` → `.produce_action()`
6. **Error handling**: Basic → Enhanced with specific exception types
7. **Convenience methods**: Added (batch operations, fluent interface, etc.)

## Troubleshooting

### Common Issues and Solutions

1. **"pynlm module not found"**
   ```bash
   pip install scikit-build-core pybind11
   pip install -e .
   ```

2. **"Invalid configuration key"**
   ```python
   # Check if key exists
   if config.has("brain.neuron_count"):
       value = config.get("brain.neuron_count")
   ```

3. **"Brain not initialized"**
   ```python
   # Always initialize before use
   brain = nlm.NLMBrain(config).initialize()
   ```

4. **"Segmentation fault"**
   ```python
   # Check initialization
   try:
       brain.initialize()
   except nlm.NLMError as e:
       print(f"Initialization failed: {e}")
   ```

5. **"Method chaining not working"**
   ```python
   # Ensure methods return self for chaining
   brain = nlm.NLMBrain(config)
   brain = brain.initialize()  # Returns self
   brain = brain.step(100)     # Returns self
   ```

## Further Reading

- [Original NLM Documentation](HOW_TO_USE.md)
- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Documentation](docs/PHASE6_FINAL_AUDIT.md)

## Version Information

- Enhanced API Version: 0.1.0
- Compatible with NLM C++ library version 0.1.0
- Requires pybind11 2.11.0+
- Requires Python 3.8+

## License

MIT License

---

*For support and questions, please refer to the original NLM documentation or contact the NLM development team.*
